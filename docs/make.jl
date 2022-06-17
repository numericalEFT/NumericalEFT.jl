# push!(LOAD_PATH, "../src/")
using Documenter, NumericalEFT



makedocs(
    modules=[NumericalEFT],
    sitename="NumericalEFT.jl",
    repo="https://github.com/numericalEFT/NumericalEFT.jl/blob/{commit}{path}#{line}",
    format=Documenter.HTML(;
        prettyurls=get(ENV, "CI", "false") == "true",
        canonical="https://numericaleft.github.io/NumericalEFT.jl",
        assets=String[]
    ),
    pages=[
        "Home" => "index.md",
        "Module ReadMe" => Any[
            "Lehmann"=>"readme/Lehmann.md",
            "FeynmanDiagram"=>"readme/FeynmanDiagram.md",
            "MCIntegration"=>"readme/MCIntegration.md",
            "CompositeGrids"=>"readme/CompositeGrids.md",
            "GreenFunc"=>"readme/GreenFunc.md",
            "Atom"=>"readme/Atom.md",
        ],
        "Manual" => Any[
            "Discrete Lehmann Representation"=>"man/DLR.md",
            "Lehmann representation convention"=>"man/kernel.md",
            "Monte Carlo integrator"=>"man/MC.md",
            "Important Sampling"=>"man/important_sampling.md",
        ],
        "API reference" => Any[
            "lib/Lehmann.md",
            "lib/Feynmandiagram.md",
            "lib/MCintegration.md",
            "lib/compositegrids.md",
            "lib/greenfunc.md",
            "lib/atom.md",
            "lib/utility.md",
            "lib/fastmath.md",
        ]
    ])

deploydocs(repo="github.com/numericalEFT/NumericalEFT.jl.git")
