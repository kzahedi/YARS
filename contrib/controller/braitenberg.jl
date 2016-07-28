global t = 0

function braitenberg(n_::Ptr{Csize_t}, in_::Ptr{Cdouble},
                     m_::Ptr{Csize_t}, out_::Ptr{Cdouble},
                     l_::Ptr{Csize_t}, str_::Ptr{UInt8})

  n::Int64 = unsafe_load(n_)
  m::Int64 = unsafe_load(m_)
  l::Int64 = unsafe_load(l_)

  input::Vector{Float64}  = pointer_to_array(in_,  (n,))
  output::Vector{Float64} = pointer_to_array(out_, (m,))
  message::Vector{UInt8}  = pointer_to_array(str_, (l,))

  # START your code here

  global t

  t = t + 1
  status = 0

  if t % 1000 == 0
    status = 1
  end

  if t == 10000
    status = 2
  end

  o = [-tanh(1/3*(input[4] + input[5] + input[6])),
       -tanh(1/3*(input[1] + input[2] + input[3]))]

  str = string(input[1], " ", input[2], " ", input[3], "\n", input[4], " ", input[5], " ", input[6]) 

  # END your code here

  unsafe_copy!(pointer(output),  pointer(o),   m)
  unsafe_copy!(pointer(message), pointer(str), l)

  return convert(Cint, status)
end

function test_function()
  println("test function called")
end

cfunction(braitenberg, Cint, (Ptr{Csize_t}, Ptr{Cdouble}, Ptr{Csize_t}, Ptr{Cdouble}, Ptr{Csize_t}, Ptr{UInt8}))
