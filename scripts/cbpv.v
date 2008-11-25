# page xxxxiv of Call-by-push-value
# doing call-by-push-value in concatenative-language V
#java -jar pkg/v.jar ./scripts/cbpv.v
# output:
#    hello0
#    hello2
#    hello3
#    we just pushed 7 
#    hello1
#    we just poped 7
#    w is bound to 10
#    15


'hello0' puts
3 [x] let
[
    'hello1' puts
    [z] let
    'we just poped ' put z puts
     x z +
] [y] let
'hello2' puts
[
    'hello3' puts
    7
    'we just pushed 7 ' puts
    y i
] i [w] let
'w is bound to ' put w puts
5 w +
puts

