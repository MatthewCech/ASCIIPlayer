
cd ..\

for /d %%x in (project_*) do rd /s /q "%%x"
for /d %%x in (bin_*) do rd /s /q "%%x"
rd /s /q "documentation"
