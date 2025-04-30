gcc main.c -Werror -Wpedantic -std=c11 -I .\include\ -L .\lib\ -lraylib -lopengl32 -lgdi32 -lwinmm -o app.exe
.\app.exe