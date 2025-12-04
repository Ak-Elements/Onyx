cd core/external
call build_tree_sitter.bat
cd ..
cd ..

dotnet build ./core/onyx-codegen.csproj -c Release -o out  