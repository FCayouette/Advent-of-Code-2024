open System.IO

let toIllegal (s:string) =
    let vals = s.Split("|") |> Array.map System.Int32.Parse
    (vals[1], vals[0])

let GetMiddle (x: int array) =
    x[(Array.length x)/2]

[<EntryPoint>]
let main argv =
    let input = File.ReadAllLines(argv[0])
    let cut = Array.findIndex(fun (s:string) -> s.Length < 1) input
    let illegals = Array.map toIllegal input[..cut-1]

    let IsValidUpdate u =
        let l = Array.length u
        let t = Array.fold(fun t i -> let pairs = Array.map (fun x -> (u[i], x)) u[i+1..]
                                      let illegals = Array.filter(fun p -> Array.exists(fun x -> x=p) illegals) pairs
                                      t + Array.length illegals) 0 [|0..l-2|]
        t=0

    let updates = Array.map(fun (s:string) -> s.Split(",") |> Array.map System.Int32.Parse) input[cut+1..]
    let valids, invalids = Array.partition IsValidUpdate updates
    Array.map GetMiddle valids |> Array.sum |> printfn("Part 1: %A")

    let Fix u =
        let l = Array.length u
        for i = 0 to l/2 do
            for j = i+1 to l-1 do
                if Array.exists(fun p -> p=(u[i], u[j])) illegals then
                    let t = u[i]
                    u[i]<-u[j]
                    u[j]<-t
        GetMiddle u
    Array.map Fix invalids |> Array.sum |> printfn("Part 2: %A")
    0