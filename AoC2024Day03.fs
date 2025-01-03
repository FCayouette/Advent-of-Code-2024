open System.IO
open System.Text.RegularExpressions

let ParseMul (s:string) =
    let values = s[4..s.Length-2].Split(",") |> Array.map System.Int32.Parse
    Array.fold(fun t x -> t * x) 1 values
    
let Part2Fold (total, active) (s:string) =
    if s[0]='d' then (total, s.Length < 5)
    else if active then (total + (ParseMul s), true)
         else (total, false)

[<EntryPoint>]
let main argv =
    let input = File.ReadAllLines argv[0] |> Array.fold(fun t s -> t+s) ""
    let mulMatch = seq{ for m in Regex.Matches(input, """mul\(\d+,\d+\)""") do yield ParseMul m.Value}  
    printfn("Part 1: %A") <| Seq.sum mulMatch
    let matches = seq { for m in Regex.Matches(input, """mul\(\d+,\d+\)|do\(\)|don't\(\)""") do yield m.Value}
    printfn("Part 2: %A") <| fst (Seq.fold Part2Fold (0, true) matches)
    0