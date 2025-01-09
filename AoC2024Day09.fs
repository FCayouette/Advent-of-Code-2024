open System.IO

let ToInt64 (s:string) =
    s.ToCharArray() |> Array.map(fun c -> (int64)(c) - (int64)('0'))

let MakeDisk (i,v) =
    let id = match i%2 with | 0 -> ((int64)i)/2L | _ -> -1L
    [|for i in 0L .. v-1L -> id|]

let AsBlocks input =
    let files, emptySpaces, _ = Array.fold(fun (f, e, (isFile, index)) l -> if isFile then ((Array.append f [|index, l|]), e, (false, index+l))
                                                                            else (f, (Array.append e [|index, l|]), (true, index+l))) ([||],[||], (true,0L)) input

    (Array.indexed files, Set.ofArray (Array.filter(fun (_,s) -> s > 0L) emptySpaces))

let LastValid = Array.findIndexBack(fun (_,x) -> x>=0L)

let Part1 (disk: (int * int64) array) =
    let compressed,_ = Array.fold(fun (comp, back) (i, v) -> if v >=0L || back <= i then ((Array.append comp [|i,v|]), back)
                                                             else let b = LastValid disk
                                                                  if b < i then (Array.append comp [|i,v|], b)
                                                                  else let tupleI = (i, snd disk[b])
                                                                       let tupleB = (b, -1L)
                                                                       disk[i] <- tupleI
                                                                       disk[b] <- tupleB
                                                                       (Array.append comp [|i,snd disk[i]|], b))([||], Array.length disk) disk
    Array.filter(fun (_,x) -> x >= 0L) compressed |> Array.fold(fun t (i,v) -> t + (int64)i * v) 0L
    
let Part2 files emptyspaces = 
    let reversed = Array.rev files
    let mutable spaces = emptyspaces
    let HasSpaceBefore i l =
        let matches = Set.filter(fun (x,s) -> x<i && l<=s) spaces
        if Set.isEmpty matches then (-1L, -1L) else Set.minElement matches
    
    let X = Array.map(fun (index, (i,l)) -> let x,s = HasSpaceBefore i l
                                            if x<0L then (index, (i,l))
                                            else spaces <- Set.remove (x,s) spaces
                                                 if s>l then spaces <- Set.add (x+l, s-l) spaces
                                                 (index, (x, l))) reversed
    Array.fold(fun t (v, (x,s)) -> t + (int64)v * (s * (s-1L) / 2L + s * x)) 0L X

[<EntryPoint>]
let main argv =
    let input = File.ReadAllText(argv[0]) |> ToInt64
    let disk = Array.indexed input |> Array.fold(fun d v -> Array.append d (MakeDisk v)) [||]
    Array.indexed disk |> Part1 |> printfn("Part 1: %A") 
    let files, empty = AsBlocks input
    Part2 files empty |> printfn("Part 2: %A")
    0