open System.IO

let IsValid (values: int array) =
    let initDiff = values[1]-values[0]
    Array.fold(fun b (x, y) -> let diff = y-x
                               b && (diff) * initDiff > 0 && abs(y-x) <= 3 && abs(y-x) > 0) true (Array.pairwise values)

let ArrayWithout (a: int array) i =
    match i with | 0 -> a[1..]
                 | _ -> Array.append a[..i-1] a[i+1..]

let IsTolerable(values: int array) =
    Array.fold(fun b i -> b || (IsValid (ArrayWithout values i))) false [|0..(values.Length-1)|]

[<EntryPoint>]
let main argv =
    let data = File.ReadLines(argv[0]) |> Seq.map (fun (s:string) -> s.Split(' ') |> Array.map System.Int32.Parse)
    let part1 = Seq.filter IsValid data |> Seq.length
    printfn("Part 1: %A") part1
    let Added = Seq.filter(fun x -> not (IsValid x)) data |> Seq.filter IsTolerable |> Seq.length
    printfn("Part 2: %A") (part1 + Added)
    0