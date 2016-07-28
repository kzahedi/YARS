package org.yars.examples;

import org.yars.communication.*;

public class Braitenberg
{

  public static double tanh(double x)
  {
    return (Math.exp(x)-Math.exp(-x))/(Math.exp(x)+Math.exp(-x));
  }

  public static void main(String argv[])
  {
    double l1          = 0;
    double l2          = 0;
    double l3          = 0;
    double r1          = 0;
    double r2          = 0;
    double r3          = 0;

    double leftInput   = 0;
    double rightInput  = 0;

    double leftOutput  = 0;
    double rightOutput = 0;

    int    count       = 0;

    try
    {
      YarsClientCom ycc = new YarsClientCom();
      ycc.connect();
      do
      {
        ycc.update();
        l1 = ycc.getSensorValue(0, 0);
        l2 = ycc.getSensorValue(1, 0);
        l3 = ycc.getSensorValue(2, 0);
        r1 = ycc.getSensorValue(3, 0);
        r2 = ycc.getSensorValue(4, 0);
        r3 = ycc.getSensorValue(5, 0);
        leftInput   = (l1 + l2 + l3) / 3.0;
        rightInput  = (r1 + r2 + r3) / 3.0;
        leftOutput  = Braitenberg.tanh(-2 * rightInput);
        rightOutput = Braitenberg.tanh(-2 * leftInput);
        ycc.setMotorValue(0, 0, leftOutput);
        ycc.setMotorValue(1, 0, rightOutput);
        count++;
        if(count > 10000)
        {
          ycc.sendQuit();
        }
        if(count % 500 == 0)
        {
          ycc.sendReset();
        }
      } while(ycc.reset());
    }
    catch(CommunicationException ce)
    {
      ce.printStackTrace();
    }
  }
}
