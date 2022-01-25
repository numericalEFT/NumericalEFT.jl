var documenterSearchIndex = {"docs":
[{"location":"lib/utility/#Utility","page":"Utility","title":"Utility","text":"","category":"section"},{"location":"lib/utility/","page":"Utility","title":"Utility","text":"Modules = [NumericalEFT.Utility]","category":"page"},{"location":"lib/utility/#NumericalEFT.Utility","page":"Utility","title":"NumericalEFT.Utility","text":"Utility data structures and functions\n\n\n\n\n\n","category":"module"},{"location":"lib/utility/#NumericalEFT.Utility.StopWatch","page":"Utility","title":"NumericalEFT.Utility.StopWatch","text":"StopWatch(start, interval, callback)\n\nInitialize a stopwatch. \n\nArguments\n\nstart::Float64: initial time (in seconds)\ninterval::Float64 : interval to click (in seconds)\ncallback : callback function after each click (interval seconds)\n\n\n\n\n\n","category":"type"},{"location":"lib/utility/#NumericalEFT.Utility.check-Tuple{NumericalEFT.Utility.StopWatch, Vararg{Any}}","page":"Utility","title":"NumericalEFT.Utility.check","text":"check(stopwatch, parameter...)\n\nCheck stopwatch. If it clicks, call the callback function with the unpacked parameter\n\n\n\n\n\n","category":"method"},{"location":"lib/utility/#NumericalEFT.Utility.progressBar-Tuple{Any, Any}","page":"Utility","title":"NumericalEFT.Utility.progressBar","text":"progressBar(step, total)\n\nReturn string of progressBar (step/total*100%)\n\n\n\n\n\n","category":"method"},{"location":"man/important_sampling/#Important-Sampling","page":"Monte Carlo integrator","title":"Important Sampling","text":"","category":"section"},{"location":"man/important_sampling/","page":"Monte Carlo integrator","title":"Monte Carlo integrator","text":"Consider the MC sampling of an one-dimensional functions f(x) (its sign may oscillate).","category":"page"},{"location":"man/important_sampling/","page":"Monte Carlo integrator","title":"Monte Carlo integrator","text":"We want to design an efficient algorithm to calculate the integral int_a^b dx f(x). To do that, we normalize the integrand with an ansatz g(x)0 to reduce the variant. ","category":"page"},{"location":"man/important_sampling/","page":"Monte Carlo integrator","title":"Monte Carlo integrator","text":"Our package supports two important sampling schemes. ","category":"page"},{"location":"man/important_sampling/#Approach-1:-Algorithm-with-a-Normalization-Section","page":"Monte Carlo integrator","title":"Approach 1: Algorithm with a Normalization Section","text":"","category":"section"},{"location":"man/important_sampling/","page":"Monte Carlo integrator","title":"Monte Carlo integrator","text":"In this approach, the configuration spaces consist of two sub-spaces: the physical sector with orders nge 1 and the normalization sector with the order n=0. The weight function of the latter, g(x), should be simple enough so that the integral G=int g(x) d x is explicitly known. In our algorithm we use a constant g(x) propto 1 for simplicity. In this setup, the physical sector weight, namely the integral F = int f(x) dx, can be calculated with the equation","category":"page"},{"location":"man/important_sampling/","page":"Monte Carlo integrator","title":"Monte Carlo integrator","text":"    F=fracF_rm MCG_rm MC G","category":"page"},{"location":"man/important_sampling/","page":"Monte Carlo integrator","title":"Monte Carlo integrator","text":"where the MC estimators F_rm MC and G_rm MC are measured with ","category":"page"},{"location":"man/important_sampling/","page":"Monte Carlo integrator","title":"Monte Carlo integrator","text":"F_rm MC =frac1N left sum_i=1^N_f fracf(x_i)rho_f(x_i) + sum_i=1^N_g 0 right","category":"page"},{"location":"man/important_sampling/","page":"Monte Carlo integrator","title":"Monte Carlo integrator","text":"and","category":"page"},{"location":"man/important_sampling/","page":"Monte Carlo integrator","title":"Monte Carlo integrator","text":"G_rm MC =frac1N leftsum_i=1^N_f 0 + sum_i=1^N_g fracg(x_i)rho_g(x_i)  right","category":"page"},{"location":"man/important_sampling/","page":"Monte Carlo integrator","title":"Monte Carlo integrator","text":"The probability density of a given configuration is proportional to rho_f(x)=f(x) and rho_g(x)=g(x), respectively. After N MC updates, the physical sector is sampled for N_f times, and the normalization sector is for N_g times. ","category":"page"},{"location":"man/important_sampling/","page":"Monte Carlo integrator","title":"Monte Carlo integrator","text":"Now we estimate the statistic error. According to the propagation of uncertainty, the variance of F  is given by","category":"page"},{"location":"man/important_sampling/","page":"Monte Carlo integrator","title":"Monte Carlo integrator","text":" fracsigma^2_FF^2 =  fracsigma_F_rm MC^2F_MC^2 + fracsigma_G_rm MC^2G_MC^2 ","category":"page"},{"location":"man/important_sampling/","page":"Monte Carlo integrator","title":"Monte Carlo integrator","text":"where sigma_F_rm MC and sigma_G_rm MC are variance of the MC integration F_rm MC and G_rm MC, respectively. In the Markov chain MC, the variance of F_rm MC can be written as ","category":"page"},{"location":"man/important_sampling/","page":"Monte Carlo integrator","title":"Monte Carlo integrator","text":"sigma^2_F_rm MC = frac1N left sum_i^N_f left( fracf(x_i)rho_f(x_i)- fracFZright)^2 +sum_j^N_g left(0-fracFZ right)^2  right ","category":"page"},{"location":"man/important_sampling/","page":"Monte Carlo integrator","title":"Monte Carlo integrator","text":"= int left( fracf(x)rho_f(x) - fracFZ right)^2 fracrho_f(x)Z rm dx + int left( fracFZ right)^2 fracrho_g(x)Z dx ","category":"page"},{"location":"man/important_sampling/","page":"Monte Carlo integrator","title":"Monte Carlo integrator","text":"=  int fracf^2(x)rho_f(x) fracdxZ -fracF^2Z^2 ","category":"page"},{"location":"man/important_sampling/","page":"Monte Carlo integrator","title":"Monte Carlo integrator","text":"Here Z=Z_f+Z_g and Z_fg=int rho_fg(x)dx are the partition sums of the corresponding configuration spaces. Due to the detailed balance, one has Z_fZ_g=N_fN_g.  ","category":"page"},{"location":"man/important_sampling/","page":"Monte Carlo integrator","title":"Monte Carlo integrator","text":"Similarly, the variance of G_rm MC can be written as ","category":"page"},{"location":"man/important_sampling/","page":"Monte Carlo integrator","title":"Monte Carlo integrator","text":"sigma^2_G_rm MC=  int fracg^2(x)rho_g(x) fracrm dxZ - fracG^2Z^2","category":"page"},{"location":"man/important_sampling/","page":"Monte Carlo integrator","title":"Monte Carlo integrator","text":"By substituting rho_f(x)=f(x) and  rho_g(x)=g(x), the variances of F_rm MC and G_rm MC are given by","category":"page"},{"location":"man/important_sampling/","page":"Monte Carlo integrator","title":"Monte Carlo integrator","text":"sigma^2_F_rm MC= frac1Z^2 left( Z Z_f - F^2 right)","category":"page"},{"location":"man/important_sampling/","page":"Monte Carlo integrator","title":"Monte Carlo integrator","text":"sigma^2_G_rm MC= frac1Z^2 left( Z Z_g - G^2 right)","category":"page"},{"location":"man/important_sampling/","page":"Monte Carlo integrator","title":"Monte Carlo integrator","text":"We derive the variance of F as","category":"page"},{"location":"man/important_sampling/","page":"Monte Carlo integrator","title":"Monte Carlo integrator","text":"fracsigma^2_FF^2 = fracZ cdot Z_fF^2+fracZ cdot Z_gG^2 - 2 ","category":"page"},{"location":"man/important_sampling/","page":"Monte Carlo integrator","title":"Monte Carlo integrator","text":"Note that g(x)0 indicates Z_g = G,  so that","category":"page"},{"location":"man/important_sampling/","page":"Monte Carlo integrator","title":"Monte Carlo integrator","text":"fracsigma^2_FF^2 = fracZ_f^2F^2+fracGcdot Z_fF^2+fracZ_fG - 1","category":"page"},{"location":"man/important_sampling/","page":"Monte Carlo integrator","title":"Monte Carlo integrator","text":"Interestingly, this variance is a function of G instead of a functional of g(x). It is then possible to normalized g(x) with a constant to minimize the variance. The optimal constant makes G to be,","category":"page"},{"location":"man/important_sampling/","page":"Monte Carlo integrator","title":"Monte Carlo integrator","text":"fracd sigma^2_FdG=0","category":"page"},{"location":"man/important_sampling/","page":"Monte Carlo integrator","title":"Monte Carlo integrator","text":"which makes G_best = F. The minimized the variance is given by,","category":"page"},{"location":"man/important_sampling/","page":"Monte Carlo integrator","title":"Monte Carlo integrator","text":"fracsigma^2_FF^2= left(fracZ_fF+1right)^2 - 2ge 0","category":"page"},{"location":"man/important_sampling/","page":"Monte Carlo integrator","title":"Monte Carlo integrator","text":"The equal sign is achieved when f(x)0 is positively defined.","category":"page"},{"location":"man/important_sampling/","page":"Monte Carlo integrator","title":"Monte Carlo integrator","text":"It is very important that the above analysis is based on the assumption that the autocorrelation time negligible. The autocorrelation time related to the jump between the normalization and physical sectors is controlled by the deviation of the ratio f(x)g(x) from unity. The variance sigma_F^2 given above will be amplified to sim sigma_F^2 tau where tau is the autocorrelation time.","category":"page"},{"location":"man/important_sampling/#Approach-2:-Conventional-algorithm-(e.g.,-Vegas-algorithm)","page":"Monte Carlo integrator","title":"Approach 2: Conventional algorithm (e.g., Vegas algorithm)","text":"","category":"section"},{"location":"man/important_sampling/","page":"Monte Carlo integrator","title":"Monte Carlo integrator","text":"Important sampling is actually more straightforward than the above approach. One simply sample x with a distribution rho_g(x)=g(x)Z_g, then measure the observable f(x)g(x). Therefore, the mean estimation,","category":"page"},{"location":"man/important_sampling/","page":"Monte Carlo integrator","title":"Monte Carlo integrator","text":"fracFZ=int dx fracf(x)g(x) rho_g(x)","category":"page"},{"location":"man/important_sampling/","page":"Monte Carlo integrator","title":"Monte Carlo integrator","text":"the variance of F in this approach is given by,","category":"page"},{"location":"man/important_sampling/","page":"Monte Carlo integrator","title":"Monte Carlo integrator","text":"sigma_F^2=Z_g^2int dx left( fracf(x)g(x)- fracFZ_gright)^2rho_g(x)","category":"page"},{"location":"man/important_sampling/","page":"Monte Carlo integrator","title":"Monte Carlo integrator","text":"fracsigma_F^2F^2=fracZ_gF^2int dx fracf(x)^2g(x)- 1","category":"page"},{"location":"man/important_sampling/","page":"Monte Carlo integrator","title":"Monte Carlo integrator","text":"The optimal g(x) that minimizes the variance is g(x) =f(x),","category":"page"},{"location":"man/important_sampling/","page":"Monte Carlo integrator","title":"Monte Carlo integrator","text":"fracsigma_F^2F^2=fracZ_f^2F^2-1","category":"page"},{"location":"man/important_sampling/","page":"Monte Carlo integrator","title":"Monte Carlo integrator","text":"The variance of the conventional approach is a functional of g(x), while that of the previous approach isn't. There are two interesting limit:\nIf the f(x)0, the optimal choice g(x)=f(x) leads to zero variance. In this limit, the conventional approach is clearly much better than the previous approach.\nOn the other hand, if g(x) is far from the optimal choice f(x), say simply setting g(x)=1, one naively expect that the the conventional approach may leads to much larger variance than the previous approach. However,  this statement may not be true. If g(x) is very different from f(x), the normalization and the physical sector in the previous approach mismatch, causing large autocorrelation time and large statistical error . In contrast, the conventional approach doesn't have this problem.","category":"page"},{"location":"man/important_sampling/#Benchmark","page":"Monte Carlo integrator","title":"Benchmark","text":"","category":"section"},{"location":"man/important_sampling/","page":"Monte Carlo integrator","title":"Monte Carlo integrator","text":"To benchmark, we sample the following integral up to 10^8 updates, ","category":"page"},{"location":"man/important_sampling/","page":"Monte Carlo integrator","title":"Monte Carlo integrator","text":"int_0^beta e^-(x-beta2)^2delta^2dx approx sqrtpidelta","category":"page"},{"location":"man/important_sampling/","page":"Monte Carlo integrator","title":"Monte Carlo integrator","text":"where beta gg delta.","category":"page"},{"location":"man/important_sampling/","page":"Monte Carlo integrator","title":"Monte Carlo integrator","text":"g(x)=f(x)","category":"page"},{"location":"man/important_sampling/","page":"Monte Carlo integrator","title":"Monte Carlo integrator","text":"Normalization Sector:  doesn't lead to exact result, the variance left(fracZ_fF+1right)^2 - 2=2 doesn't change with parameters","category":"page"},{"location":"man/important_sampling/","page":"Monte Carlo integrator","title":"Monte Carlo integrator","text":"beta 10 100\nresult 0.1771(1) 0.1773(1)","category":"page"},{"location":"man/important_sampling/","page":"Monte Carlo integrator","title":"Monte Carlo integrator","text":"Conventional: exact result","category":"page"},{"location":"man/important_sampling/","page":"Monte Carlo integrator","title":"Monte Carlo integrator","text":"g(x)=sqrtpideltabeta1","category":"page"},{"location":"man/important_sampling/","page":"Monte Carlo integrator","title":"Monte Carlo integrator","text":"beta 10 100\nNormalization 0.1772(4) 0.1767(17)\nConventional 0.1777(3) 0.1767(8)","category":"page"},{"location":"man/important_sampling/","page":"Monte Carlo integrator","title":"Monte Carlo integrator","text":"g(x)=exp(-(x-beta2+s)^2delta^2) with beta=100","category":"page"},{"location":"man/important_sampling/","page":"Monte Carlo integrator","title":"Monte Carlo integrator","text":"s delta 2delta 3delta 4delta 5delta\nNormalization 0.1775(8) 0.1767(25) 0.1770(60) 0.176(15) 183(143)\nConventional 0.1776(5) 0.1707(39) 0.1243(174) 0.0204 (64) ","category":"page"},{"location":"man/important_sampling/","page":"Monte Carlo integrator","title":"Monte Carlo integrator","text":"The conventional algorithm is not ergodic anymore for s=4delta, the acceptance ratio to update x is about 015, while the normalization algorithm becomes non ergodic for s=5delta. So the latter is slightly more stable.","category":"page"},{"location":"man/important_sampling/","page":"Monte Carlo integrator","title":"Monte Carlo integrator","text":"<!– The code are ![[test.jl]] for the normalization approach and ![[test2.jl]] for the conventional approach. –>","category":"page"},{"location":"man/important_sampling/","page":"Monte Carlo integrator","title":"Monte Carlo integrator","text":"Reference:  [1] Wang, B.Z., Hou, P.C., Deng, Y., Haule, K. and Chen, K., Fermionic sign structure of high-order Feynman diagrams in a many-fermion system. Physical Review B, 103, 115141 (2021).","category":"page"},{"location":"#NumericalEFT.jl","page":"Home","title":"NumericalEFT.jl","text":"","category":"section"},{"location":"","page":"Home","title":"Home","text":"Numerical effective field theory approach to quantum many-body systems.","category":"page"},{"location":"#Manual-Outline","page":"Home","title":"Manual Outline","text":"","category":"section"},{"location":"","page":"Home","title":"Home","text":"Pages = [\n\"man/important_sampling.md\"\n]\nDepth = 1","category":"page"},{"location":"#Library-Outline","page":"Home","title":"Library Outline","text":"","category":"section"},{"location":"","page":"Home","title":"Home","text":"Pages = [\n    \"lib/fastmath.md\",\n    \"lib/utility.md\",\n]\nDepth = 1","category":"page"},{"location":"lib/fastmath/#Fast-Math-Functions","page":"Fast Math Functions","title":"Fast Math Functions","text":"","category":"section"},{"location":"lib/fastmath/","page":"Fast Math Functions","title":"Fast Math Functions","text":"Modules = [NumericalEFT.FastMath]","category":"page"},{"location":"lib/fastmath/#NumericalEFT.FastMath","page":"Fast Math Functions","title":"NumericalEFT.FastMath","text":"Provide a set of fast math functions\n\n\n\n\n\n","category":"module"},{"location":"lib/fastmath/#NumericalEFT.FastMath.invsqrt-Tuple{Float64}","page":"Fast Math Functions","title":"NumericalEFT.FastMath.invsqrt","text":"invsqrt(x)\n\nThe Legendary Fast Inverse Square Root See the following links: wikipedia and thesis\n\n\n\n\n\n","category":"method"}]
}
