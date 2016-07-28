function [] =  sample( varargin )
  tic

  p = inputParser;
  p.addRequired('filename',  @ischar);
  p.addRequired('sensors',   @isvector);
  p.addRequired('actuators', @isvector);

  p.addParamValue('bins',        10, @isscalar);
  p.addParamValue('cols',        [], @isnumeric);
  p.addParamValue('row',         [], @isnumeric);
  p.addParamValue('verbose',  false, @islogical);
  p.parse(varargin{:});
  p.Results;

  filename  = p.Results.filename;
  sensors   = p.Results.sensors;
  actuators = p.Results.actuators;
  bins      = p.Results.bins;


  data = readData(p.Results.filename, 'cols', p.Results.cols, 'row', p.Results.row, 'verbose', p.Results.verbose);

  data = discretiseMatrix(data, bins);


  combinedData = zeros(size(data,1), 2);

  for row = 1:size(data,1)
    s_value = 0;
    a_value = 0;
    for s = 1:size(sensors,2)
      s_value = s_value + (bins .^ (s-1)) * (data(row,sensors(s)) - sign(s-1));
    end
    for a = 1:size(actuators,2)
      a_value = a_value + (bins .^ (a-1)) * (data(row,actuators(a)) - sign(a-1));
    end
    combinedData(row,1) = s_value;
    combinedData(row,2) = a_value;
  end

  sDim = max(bins .^ size(sensors,2));
  aDim = max(bins .^ size(actuators,2));

  policy             = cpm(sDim, aDim);
  sensorDistribution = cpm(1, sDim);
  worldModel         = cpm(sDim * aDim, sDim);

  for row = 2:size(combinedData, 1)
    s      = combinedData(row-1,1);
    a      = combinedData(row-1,2);
    sPrime = combinedData(row  ,1);

    policy             = policy.update(s,a);
    sensorDistribution = sensorDistribution.update(1,s);
    worldModel         = worldModel.update((s-1) * aDim + a, sPrime);
  end

  f_policy              = sprintf('%s-policy.txt',              filename);
  f_sensor_distribution = sprintf('%s-sensor_distribution.txt', filename);
  f_world_model         = sprintf('%s-world_model.txt',         filename);
  dlmwrite(f_policy,              policy.m,             ' ');
  dlmwrite(f_sensor_distribution, sensorDistribution.m, ' ');
  dlmwrite(f_world_model,         worldModel.m,         ' ');
  
  toc
end
