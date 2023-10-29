let x = 5;
{
function f(y) {
return (x+y)-2;
}
{
function g(h) {
let x = 7;
return h(x);
}
{
let x = 10;
console.log(g(f));
  }
}
}