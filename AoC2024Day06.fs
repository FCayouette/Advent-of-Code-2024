open System.IO

let ProcessLine (s:string) =
    s.ToCharArray() |> Array.indexed |> Array.filter(fun (_,c) -> c='#') |> Array.map fst

let ExtractObstacles grid =
    Array.indexed grid |> Array.fold(fun obs (x, (s:string)) -> Array.append obs (ProcessLine s |> Array.map(fun y -> (x,y)))) [||] |> Set

let directions = [|(-1,0); (0,1); (1,0); (0,-1)|]

[<EntryPoint>]
let main argv = 
    let grid = File.ReadAllLines(argv[0])
    let maxX, maxY = Array.length grid, String.length grid[0]
    let startX = Array.findIndex(fun (s:string) -> s.Contains('^')) grid
    let start = startX,Array.findIndex(fun c -> c='^') (grid[startX].ToCharArray())
    let obstacles = ExtractObstacles grid

    let Inside (x,y) =
        x>=0 && x<maxX && y>=0 && y<maxY

    let rec Navigate pos dir path =
        let next =  (fst pos + fst directions[dir], snd pos + snd directions[dir])
        if not (Inside next) then Set.add pos path
        else if obstacles.Contains(next) then Navigate pos ((dir+1)%4) path
             else Navigate next dir (Set.add pos path)
    
    let path = Navigate start 0 Set.empty
    Set.toArray path |> Array.length |> printfn("Part 1: %A")
    
    let rec LoopCheck additional pos dir (track: Set<(int*int)*int>) = 
        let state = (pos, dir)
        if track.Contains(state) then 1
        else let next = (fst pos + fst directions[dir], snd pos + snd directions[dir])
             if not (Inside next) then 0
             else if next=additional || obstacles.Contains(next) then LoopCheck additional pos ((dir+1)%4) (Set.add state track)
                  else LoopCheck additional next dir (Set.add state track)

    Set.fold(fun t pos -> t + LoopCheck pos start 0 Set.empty) 0 (Set.remove start path) |> printfn("Part 2: %A")
    0