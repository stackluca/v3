// binary
var a = 1+1;

function a(){var x=1; return x;}

// return 
function a(){return 1;}
a()

// argument
function a(x){return x;}
a(1)

function sum(x, y){return x+y}
sum(3,5)

// closure
function sumbase(base){return function(x){return x+base;} }
var sum3=sumbase(3);
sum3(5); 

var sum4=sumbase(4);
sum4(5);

// type conversion
var a =1
a.toString()

// object

// prototype
function Rectangle(w, h){this.width = x; this.height = h}
var r = new Rectangle(3,5)
r
r.area()
Rectangle.prototype.area = function(){return this.x * this.y}
r.area()

// exception



