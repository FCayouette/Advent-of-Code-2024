open System.IO

let ToGrid (s:string) =
    s.ToCharArray() |> Array.map(fun c -> (int)c - (int)'0')

let directions = [|(1,0); (0,1); (-1,0); (0,-1)|]
    
[<EntryPoint>]
let main argv =
    let grid = File.ReadAllLines(argv[0]) |> Array.map ToGrid
    let MaxX, MaxY = Array.length grid, Array.head grid |> Array.length
    let Within (x,y) = x>=0 && y>=0 && x<MaxX && y<MaxY

    let rec CountTrails (x,y) level =
        if not (Within (x,y)) || grid[x][y] <> level then []
        else if level=9 then [(x,y)]
             else Array.fold(fun points (dx,dy) -> List.append points (CountTrails (x+dx,y+dy) (level+1))) [] directions

    let rec CountPaths (x,y) level =
        if not (Within (x,y)) || grid[x][y] <> level then 0
        else if level=9 then 1
            else Array.fold(fun t (dx,dy) -> t + (CountPaths (x+dx, y+dy) (level+1))) 0 directions

    let Process f = Array.fold(fun total (x, line) -> total + Array.fold(fun t (y,v) -> t + (f (x,y))) 0 (Array.indexed line)) 0 (Array.indexed grid)
    let Part1 pos = List.distinct (CountTrails pos 0) |> List.length
    let Part2 pos = CountPaths pos 0
    printfn("Part 1: %A") <| Process Part1
    printfn("Part 2: %A") <| Process Part2
    0