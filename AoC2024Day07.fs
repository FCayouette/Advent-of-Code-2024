open System.IO

let ProcessInput (s:string) =
    let x = s.Split(": ")
    let components = x[1].Split(" ") |> Array.map System.Int64.Parse |> Array.toList
    System.Int64.Parse x[0], components

let rec AddMul t toDate toProcess =
    if t < toDate then false
    else if List.isEmpty toProcess then t=toDate
         else let next = List.head toProcess
              let tail = List.tail toProcess
              if AddMul t (toDate + next) tail then true
              else AddMul t (toDate * next) tail

let Concat a b =
    let work = [|a; b|]
    while work[1] > 0L do
        work[0] <- work[0] * 10L
        work[1] <- work[1] / 10L
    work[0] + b

let rec AddMulCat t toDate toProcess =
    if t < toDate then false
    else if List.isEmpty toProcess then t=toDate
         else let next = List.head toProcess
              let tail = List.tail toProcess
              if AddMulCat t (toDate + next) tail then true
              else if AddMulCat t (toDate * next) tail then true
                   else AddMulCat t (Concat toDate next) tail

let Partition (t, components) recFun =
    recFun t (List.head components) (List.tail components)

[<EntryPoint>]
let main argv =
    let data = File.ReadAllLines(argv[0]) |> Array.map ProcessInput
    let addMul, other = Array.partition(fun x -> Partition x AddMul) data
    let part1 = Array.map fst addMul |> Array.sum
    printfn("Part 1: %A") part1
    let withConcat = Array.filter(fun x -> Partition x AddMulCat) other |> Array.map fst |> Array.sum
    printfn("Part 2: %A") (part1 + withConcat)
    0