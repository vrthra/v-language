=====================================
CLOS & MOP kind. Just thoughts on the subject.
==================================
defclass:
[name [list-of-supreclasses]
    [
        [slot]
        [slot]
    ]
] defclass

eg:
[cat []
    [
        [n-legs]
        [name]
    ]
] cat

[cat] make-instance
=><cat {4003360B5}>

['foofoo' <cat-{instance}> name] slot-value
[<cat-{instance}> name] slot-value

=========================================
#inheritance

[animal []
    [[alive?]]
] defclass

[mammal []
    [[environment]]
] defclass

[cat [animal mammal]
    [[n-legs] [name]]
] defclass

[cat] make-instance
[true <cat-{instance}> alive?] slot-value

#return what is in alive? slot.
[<cat-{instance}> alive?] slot-value

[cat []
    [
        [name :initarg :give-name
            :type string
            :reader get-name]
        [age :initform 0] 
        [owner :accesor owner
            :allocation :class
            :documentation 'owner of all cats']
        [state :writer secret-state]
    ]
] defclass

=============================================
#using slots.
[cat :give-name 'foofoo'] make-instance

[<cat-{instance}> get-name] g
=>'foofoo'

['fooowner' <cat-{instance}> owner] g
[<cat-{instance}> owner] g
=>'fooowner'

[cat :give-name 'fifi'] make-instance
[<cat-{instance}> owner] g
=>'fooowner'

['dead' <cat-{instance}> secret-state] g

===============================================
#generic methods.

[methodname [param1 [param2 :type]] .. ] defmethod

eg:
[get-age [[obj cat]]
    [obj age] slot-value] defmethod

[years-left [[obj cat]]
    15 [obj age] slot-value -] defmethod


================================================
#multi dispatch.

[circle [2d-coord]
    [[radius :accessor my-radius]]
] defclass

[rectangle [2d-coord]
    [
        [length :accessor my-length]
        [radius :accessor my-width]
    ]
] defclass

[area [[obj circle]]
    pi [obj my-radius] g sqr *
] defmethod

[area [[obj rectangle]]
    [obj my-width] g [obj my-length] g *
] defmethod

===================================================
# generic functions.
(defgeneric Name (param1 param2)
    (:documentation "A sample generic function")
    (:method ((obj1 Type1) (obj2 Type2)) (function body))
    (:method-combination progn)
    (:argument-precedence-order param2 param1))


[name [param1 param2]
    [:documentation 'a sample generic function']
    [:method [[obj1 type1] [obj2 type2]] [function body.]]
    [:method-combination progn]
    [:argument-precedence-order param2 param1] 
] defgeneric

===================================================
# method combination

#primary method
[method-name [[n number]] n]  defmethod

#before methods
[method-name :before [[n integer]]
    'integer'] defmethod

[method-name :before [[n rational]]
    'rational'] defmethod

#after methods.
[method-name :after [[n integer]]
    'integer'] defmethod

[method-name :after [[n rational]]
    'rational'] defmethod

#around
[method-name :around [[n integer]]
    [call-next-method]] defmethod

[method-name :around [[n rational]]
    'sorry'] defmethod

=======================================================
Totaly unrelated. perhaps the Java and Ruby kind.
=======================================================

class Cow
    horns
    moo
end

[Cow horns moo] class
-- should define accessor functions for Cow.horns Cow.moo

they should accept a quote of form

[myhorns mymoo] and return the value correctly.

[aa bb] Cow.horns => aa

[new] Cow => [Cow$]
[Cow$ aa bb] [horns] Cow => aa
[Cow$ aa bb] [xx horns] Cow => [xx bb]

=====================================

On the other hand, the nature of 'V' makes a prototype based
system seem more closer.

[] dup



