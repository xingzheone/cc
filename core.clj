;(get {3 :fe 4 "ha" 6 99} 3) 有bug
(def! type-str
  (fn* [x] (nth 
    ["unkown-type" "symbol" "key" "int" "float" "string" "true" "false" "nil" "list" "vector" "map" "native" "fn" "eror" "atom"]
    x
    ))
)

(prn "xingzhe: " (type-str (type '(a hao))))
