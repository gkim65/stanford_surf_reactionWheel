using LibSerialPort, Makie
using Flux
using Crux
using BSON: @load
using Dates

cd("C:/Users/grace/Documents/IMPORTANT_Documents/Internships/SURF/")

@load "ppo.bson" pi_ppo


portname = "COM3"
motorportname = "COM9"
baudrate = 115200

TIME_LIMIT = Second(180)
start = Dates.now()
time_elapsed = Second(0)

println("-------------------- SESSION -------------------------")

while (time_elapsed < TIME_LIMIT)
    LibSerialPort.open(portname, baudrate) do sp
        
        sleep(0.1)
        println("NEW READING")
        while bytesavailable(sp) > 0
            s = String(readline(sp))
            println("Julia - I read:", s)
            data = split(s, ":")
            if size(data, 1) == 4
                
                if data[1] == "GyroZ" && data[3] == "OrientationZ" && tryparse(Float64, data[2]) !== nothing && tryparse(Float64, data[4]) !== nothing               
                    theta = parse(Float64, data[4])-90.
                    omega = parse(Float64, data[2])
                    println("theta: ", theta, " omega: ",omega)
                    action = pi_ppo(theta,omega)[1]
                    action_clamp = clamp(action, -2, 2)
                    println("Action: ", action, " clamp: ", action_clamp)
                    # write(sp, abs(action_clamp))
                    if action_clamp >= 0
                        write(sp, 1)
                    else
                        write(sp, 2)
                    end
#                    write(sp, abs(trunc(Int, action_clamp)))
                    
                end
            end
        end

        # write(sp, 255)
        # sleep(0.1)
        # println("Julia - I read:", readline(sp))
    end
    global time_elapsed = Dates.now() - start
    println(time_elapsed)
end