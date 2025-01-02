open System.IO

let toPair (s:string) =
    let ints = s.Split(' ') |> Array.filter(fun (x:string) -> x.Length > 0) |> Array.map System.Int32.Parse
    ints[0], ints[1]

let GetCountOf array v =
    snd (Array.find(fun x -> v = fst x) array)

[<EntryPoint>]
let main argv = 
    let data = File.ReadLines(argv[0]) |> Seq.map toPair
    let l, r = Seq.fold(fun (l, r) (pair) -> Array.append l [|fst pair|], Array.append r [|snd pair|]) ([||], [||]) data
    printfn("Part 1: %A") <| Array.fold2(fun t a b -> t + abs(a-b)) 0 (Array.sort l) (Array.sort r)
    let common = Set.intersect (Set.ofArray l) (Set.ofArray r)
    let lCount, rCount = Array.countBy id l, Array.countBy id r   
    printfn("Part 2: %A") <| Set.fold(fun t v -> t + v*(GetCountOf lCount v)*(GetCountOf rCount v)) 0 common
    0