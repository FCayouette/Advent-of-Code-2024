open System.IO

let ProcessLine (line: int*string) c =
    let x, s = line
    s.ToCharArray() |> Array.indexed |> Array.fold(fun coords (y, a) -> if a=c then Array.append coords [|(x,y)|] 
                                                                               else coords ) [||]
let ListAll grid c =
    Array.fold(fun coords x -> Array.append coords (ProcessLine x c)) [||] grid

let searchDirs = [|(-1,-1); (-1,0); (-1, 1); (0, -1); (0, 1); (1, -1); (1,0); (1,1)|]
let downDirs = [|(-1,-1); (1,1)|]
let upDirs = [| (-1, 1); (1, -1)|]

let CoordAdd (x,y) (dx,dy) = (x+dx, y+dy)

[<EntryPoint>]
let main argv =
    let indexedData = File.ReadAllLines(argv[0]) |> Array.indexed
    let X = ListAll indexedData 'X' 
    let M = ListAll indexedData 'M' |> Set
    let A = ListAll indexedData 'A' |> Set
    let S = ListAll indexedData 'S' |> Set

    let MASCheck pos delta =
        let mPos = CoordAdd pos delta
        if not (Set.exists(fun x -> x=mPos) M) then 0
        else 
            let aPos = CoordAdd mPos delta
            if not (Set.exists(fun y -> y=aPos) A) then 0
            else
                let sPos = CoordAdd aPos delta
                if Set.exists(fun z -> z=sPos) S then 1 else 0

    printfn("Part 1: %A") <| Array.fold(fun t pos -> t + Array.fold(fun d delta -> d + (MASCheck pos delta)) 0 searchDirs) 0 X

    let DirCheck pos (dir: (int * int) array) =
        let a = CoordAdd pos dir[0]
        let b = CoordAdd pos dir[1]
        let AorB x = x=a || x=b
        Set.exists AorB M && Set.exists AorB S

    let XCheck pos =
        if DirCheck pos upDirs && DirCheck pos downDirs then 1 else 0

    printfn("Part 2: %A") <| Set.fold(fun t pos -> t + XCheck pos) 0 A
    0