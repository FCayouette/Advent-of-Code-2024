open System.IO

let Powers = [|for i in 0L .. 18L -> (int64)((double)10.0 ** (double)i)|]
let GetPower x = Array.findIndex(fun p -> x < p) Powers

let AddToMap (t:Map<int64,int64>) k v = 
   if t.ContainsKey k then 
        let f x = match x with | Some s -> Some (s+v) | None -> None
        t.Change(k, f)
   else t.Add(k, v) 

let NextState state =
    let emptyMap = Map.empty
    Map.fold(fun (t:Map<int64,int64>) k v -> 
        if k=0L then AddToMap t 1L v
        else
            let power = GetPower k
            if (power%2)=1 then AddToMap t (k*2024L) v
            else let split = power/2
                 let k1, k2 = k % Powers[split], k/Powers[split]
                 AddToMap (AddToMap t k1 v) k2 v
        ) emptyMap state

let rec Process state depth =
    if depth=0 then Map.fold(fun t _ c -> t + c) 0L state
    else Process (NextState state) (depth-1)

[<EntryPoint>]
let main argv = 
    let initState = File.ReadAllText(argv[0]).Split(" ") |> Array.map System.Int64.Parse |> Array.countBy id |> Array.map(fun (a,b) -> a,(int64)b) |> Map
    printfn("Part 1: %A") <| Process initState 25
    printfn("Part 2: %A") <| Process initState 75
    0