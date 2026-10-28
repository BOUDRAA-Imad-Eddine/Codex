/*output*/
document.getElementById("h1").innerHTML = 'Hello '+"World,";

document.write('This is a test of the function document.write()');

/*window.alert('Just a test of the famous window alert.')*/

console.log('Only giving a try to consol output')

/*window.print('testing the print')*/

/*statements*/

let x, y, z;

x = 'hello'
y = 123;
z = true;

var a;
const pi = 3.14;

/*statement : var let if return ...*/
a = y + 5 * 6 -2 / 5;
let b = y++;

let r = y % 5;
console.log(r);

/*Data Types*/
/*String and Numbers and Boolens */
let strings = 'this is a string';

let number = 123;

let bool = true;

/*Object*/


const person = {Name:'Schumi'};
person.Team = 'Benneton';

const person2 = {
    Name:'Schumi',
    Team:'Benneton'
};

console.log(person.Name);
function Person(first, last, age, eye) {
    this.firstName = first;
    this.lastName = last;
    this.age = age;
    this.eyeColor = eye;
}

const Pilot = new Person("Mika", "Hakinen", 48, "green");


/*Date object*/
const date = new Date("2024-09-10");

/*Array*/

const new_array = ['max','lando','charles']
/*functions*/

function Myfunc(){
    return;
}

/*Events*/
document.getElementById("button").onclick = function(){
    console.log(person.Name);
};

/*String Methods*/

let text = "Hello World";

let char = text.charAt(0);

let slice = text.slice(2, 5);

let text_to_upper = text.toUpperCase();

let text1 = "Hello";
let text2 = "World!";

let text3 = text1.concat(" ", text2);

console.log(text3);

let text_with_space = "   hello how are you  "

let text_without_space = text_with_space.trim();
/*trimStart() and trimEnd() */
console.log(text_without_space);

text.padStart();
text.padEnd();

text.repeat(2);

let replacement = text.replace("World", "There!");
console.log(replacement);

const textsplit = text.split(" ");
console.log(textsplit[1]);

/*Variables Substitution*/

let food = 'spaghetti'

let eat = `I wonna eat ${food}!`;
console.log(eat);


let n = 9.656;
console.log(n.toExponential(2));
console.log(n.toFixed(2));
console.log(n.toPrecision(2));

/*valueOf() to know the type and Numbers() to convert variables into numbers*/

/*Array*/

const array1 = ["Hello", 456, true];

console.log(array1[0] + " World");

document.getElementById("h3").innerHTML = array1.toString();

array1.legth //Returns the number of elements
array1.sort() // Sorts the array

// Array Methods

let array_part = array1.at(2);

// pop() removes the last element
// push() adds a new element at the end
// shift() removes the first
// unshift() adds a new element at the beginning
//delete() leaves undefined holes
// concat joins elements end-to-end and takes str as arguments
// copyWithin(a ,b) copy element from index b to index a
//flat() creates a new array with sub-array elements
//splice()add new elements at specified index //can also be used to remove
//slice() to creat a new array with a piece of an ither array
//find() and findIndex()


//Date Objects

const d = new Date();//returns the current time

let v = Math.round(4.6);
console.log(v);

let v_pow = Math.pow(v, 2);//sqrt gives square root
console.log(v_pow);

console.log(Math.floor(Math.random() * 100));

//boolean

console.log(Boolean(10 == Math.abs(-10)));

//if else and else if

//switch
switch (new Date().getDay()) {
    case 0:
      day = "Sunday";
      break;
    case 1:
      day = "Monday";
      break;
    case 2:
       day = "Tuesday";
      break;
    case 3:
      day = "Wednesday";
      break;
    case 4:
      day = "Thursday";
      break;
    case 5:
      day = "Friday";
      break;
    case 6:
      day = "Saturday";
  }


//For Loop
let F1s = "";

const F1_teams = ["Lotus", "Marussia", "Force India", "Benneton", "BMW"]

for (let i = 0; i < F1_teams.length; i++) {
    F1s += F1_teams[i] + "  ";
}

console.log(F1s);






























