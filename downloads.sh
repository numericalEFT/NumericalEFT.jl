rm -r ./src/Lehmann
rm -r ./test/Lehmann
git clone https://github.com/numericalEFT/Lehmann.jl.git temp
mv ./temp/basis ./src/basis
mv ./temp/src ./src/Lehmann
mv ./temp/test ./test/Lehmann
mv ./temp/docs/src/index.md ./docs/src/man/DLR.md
mv ./temp/docs/src/manual/* ./docs/src/man/
mv ./temp/docs/src/assets/* ./docs/src/assets/
mv ./temp/docs/src/index.md ./docs/src/readme/Lehmann.md
rm -rf temp

rm -r ./src/MCIntegration
rm -r ./test/MCIntegration
git clone https://github.com/numericalEFT/MCIntegration.jl.git temp
mv ./temp/src ./src/MCIntegration
mv ./temp/test ./test/MCIntegration
mv ./temp/docs/src/man/* ./docs/src/man/
mv ./temp/docs/src/assets/* ./docs/src/assets/
mv ./temp/README.md ./docs/src/readme/MCIntegration.md
rm -rf temp

rm -r ./src/CompositeGrids
rm -r ./test/CompositeGrids
git clone https://github.com/numericalEFT/CompositeGrids.jl.git temp
mv ./temp/src ./src/CompositeGrids
mv ./temp/test ./test/CompositeGrids
mv ./temp/docs/src/man/* ./docs/src/man/
mv ./temp/docs/src/assets/* ./docs/src/assets/
mv ./temp/README.org ./docs/src/readme/CompositeGrids.md
rm -rf temp

# You need to mannually change "using Lehmann" and "using CompositeGrids" to "using ..Lehmann" and "using ..CompositeGrids" in GreenFunc.jl
rm -r ./src/GreenFunc
rm -r ./test/GreenFunc
git clone https://github.com/numericalEFT/GreenFunc.jl.git temp
mv ./temp/src ./src/GreenFunc
mv ./temp/test ./test/GreenFunc
mv ./temp/docs/src/man/* ./docs/src/man/
mv ./temp/docs/src/assets/* ./docs/src/assets/
mv ./temp/README.md ./docs/src/readme/GreenFunc.md
rm -rf temp

rm -r ./src/FeynmanDiagram
rm -r ./test/FeynmanDiagram
git clone https://github.com/numericalEFT/FeynmanDiagram.jl.git temp
mv ./temp/src ./src/FeynmanDiagram
mv ./temp/test ./test/FeynmanDiagram
mv ./temp/assets/* ./docs/src/readme/assets/
mv ./temp/docs/src/man/* ./docs/src/man/
mv ./temp/docs/src/assets/* ./docs/src/assets/
# mv ./temp/docs/src/lib/* ./docs/src/lib/
mv ./temp/README.md ./docs/src/readme/FeynmanDiagram.md
rm -rf temp

rm -r ./src/Atom
rm -r ./test/Atom
git clone https://github.com/numericalEFT/Atom.jl.git temp
mv ./temp/src ./src/Atom
mv ./temp/test ./test/Atom
mv ./temp/docs/src/man/* ./docs/src/man/
mv ./temp/docs/src/assets/* ./docs/src/assets/
mv ./temp/README.md ./docs/src/readme/Atom.md
rm -rf temp