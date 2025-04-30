gcc main.c -Werror -Wpedantic -std=c11 -I .\include\ -L .\lib\ -lraylib -lopengl32 -lgdi32 -lwinmm -o app.exe
if($LASTEXITCODE -gt 0){
    throw "OHFUCUKEVANWHATDID YOUD O OTGOD THERESBLOODVERYEWHER"
}
.\app.exe