"""
Utility data structures and functions
"""
module Utility
export StopWatch, check, progressBar


"""
    progressBar(step, total)

Return string of progressBar (step/total*100%)
"""
function progressBar(step, total)
    barWidth = 70
    percent = round(step / total * 100.0, digits=2)
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
