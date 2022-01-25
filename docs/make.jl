# push!(LOAD_PATH, "../src/")
using Documenter, NumericalEFT



makedocs(
    modules = [NumericalEFT],
    sitename = "NumericalEFT.jl",
    repo = "https://github.com/numericalEFT/NumericalEFT.jl/blob/{commit}{path}#{line}",
    format = Documenter.HTML(;
        prettyurls = get(ENV, "CI", "false") == "true",
        canonical = "https://numericaleft.github.io/NumericalEFT.jl",
        assets = String[]
    ),
    pages = [
        "Home" => "index.md",
        "Manual" => Any[
            "Discrete Lehmann Representation"=>"man/DLR.md",
            "Lehmann representation convention"=>"man/kernel.md",
            "Monte Carlo integrator"=>"man/MC.md",
        ],
        "API reference" => Any[
            "lib/Lehmann.md",
            "lib/Feynmandiagram.md",
            "lib/MCintegration.md",
            "lib/compositegrids.md",
            "lib/greenfunc.md",
            "lib/utility.md",
            "lib/fastmath.md",
        ]
    ])

deploydocs(repo = "github.com/numericalEFT/NumericalEFT.jl.git")
