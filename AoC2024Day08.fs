open System.IO

let ProcessLine (i,(s:string)) =
    s.ToCharArray() |> Array.indexed |> Array.filter(fun (_,c) -> c<>'.')|> Array.map(fun (y,c) -> (c, (i,y)))

[<EntryPoint>]
let main argv =
    let grid = File.ReadAllLines(argv[0])
    let maxX = Array.length grid
    let maxY = String.length(Array.head grid)
    let data = Array.indexed grid |> Array.fold(fun t x -> Array.append t (ProcessLine x)) [||]
    let Within (a,b) = a>=0 && b >=0 && a<maxX && b<maxY
    
    let GetPairsOf (c, pos) =
        Array.filter(fun (a,b) -> c=a && pos<>b) data |> Array.map snd

    let Part1 (c,(x,y)) =
        GetPairsOf (c, (x,y)) |> Array.map(fun (X,Y) -> (x+2*(X-x), y+2*(Y-y))) |> Array.filter Within

    let rec Resonate (px,py) (dx,dy) =
        let n = (px+dx,py+dy)
        if Within n then n :: Resonate n (dx,dy)
        else []

    let Part2 (c, (x,y)) =
        let toCheck = GetPairsOf (c,(x,y))
        Array.fold(fun t (ax,ay) -> let nodes = Resonate (x,y) (ax-x,ay-y)
                                    Array.append t (List.toArray nodes)) [||] toCheck

    let Compute method =
        Array.fold(fun t a -> Array.append t (method a)) [||] data |> Array.distinct |> Array.length

    Compute Part1 |> printfn("Part 1: %A")
    Compute Part2 |> printfn("Part 2: %A")

    0