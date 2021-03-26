"""
Utility data structures and functions
"""
module Utility
export StopWatch, check, progressBar

"""
    StopWatch(start, interval, callback)

Initialize a stopwatch. 

# Arguments
- `start::Float64`: initial time (in seconds)
- `interval::Float64` : interval to click (in seconds)
- `callback` : callback function after each click (interval seconds)
"""
mutable struct StopWatch
    start::Float64
    interval::Float64
    f::Function
    StopWatch(_interval, callback) = new(time(), _interval, callback)
end

"""
    check(stopwatch, parameter...)

Check stopwatch. If it clicks, call the callback function with the unpacked parameter
"""
function check(watch::StopWatch, parameter...)
    now = time()
    if now - watch.start > watch.interval
        watch.f(parameter...)
        watch.start = now
    end
end

"""
    progressBar(step, total)

Return string of progressBar (step/total*100%)
"""
function progressBar(step, total)
    barWidth = 70
    percent = round(step / total * 100.0, digits = 2)
    str = "["
    pos = barWidth * percent / 100.0
    for i = 1:barWidth
        if i <= pos
            str *= "I"
        else
            str *= " "
        end
    end
    str *= "] $step/$total=$percent%"
    return str
end

end
