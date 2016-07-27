# OpenSwift
OpenSwift official mirror in GitHub.

#Open Swift 基础篇

标签（空格分隔）： swift 编程语言

![image_1aodbphlqm7l2j3vah1tcl1im5m.png-24.5kB][2]

OpenSwift是一套全新的编程语言。在介绍它之前，我们先说一下C++。

C++可谓包罗万象、博大精深。C++11标准一共有73个关键字，清楚地解释和描述C++的语言规范需要一本厚厚的【C++ Primer】。这也导致了很多C++代码艰深难读，难以维护。如果只保留下面这23个关键字，仍然能完成C++ 95%以上的工作，只不过一些代码需要变换一种写法而已。

下面是最简版的C++：
```cpp
asm false operator true
class delete new return this
bool char double float int long short void
break continue else for if while
```
可见从语义的角度，C++有很大的精简空间。C++标准委员会过度设计了很多我们大部分情况下都用不到的语言特性。C++的复杂度还来自于它的编译工具链以及标准库，比如boost和STL，不仅源代码艰深难读，过度的设计导致了大量无关的概念，庞大又臃肿、晦涩难用；还形成了复杂的编译器依赖，对不同操作系统和体系结构之间的移植造成了障碍。

我们希望有一套编程语言，既像C++一样功能强大，又像JavaScript一样亲切易读。经过两年时间的工匠打磨，我们精心打造了这样一套全新的编程语言。语义上，这套新的编程语言，有60%取材于JavaScript，20%取材于苹果的swift，10%取材于C++。功能上，这套新的编程语言既是解释器语言，也是C++转译语言，可以转译成C++，然后再编译成机器语言。解释器和转译器是由纯C++实现 ( 现只支持Linux环境 [Android|OpenWrt|Ubuntu] )。在可以预计的将来，类似于RUST，这套编程语言可以完成自举和对机器语言的直译。

----------
[TOC]

----------
##OpenSwift

我们希望这套新的编程语言能像苹果的swift一样，广泛应用在将来的移动计算的场景之中，所以给它取名为Open Swift。

项目地址：https://github.com/

技术的发展都是在围绕着解决“软件的复杂度”这个基本的需求而发展的。对于所有的问题，Open Swift都希望采用简单直观的解决方案，不会为了显示聪明而采用繁复的抽象。Open Swift没有柯里化、原型链、虚函数表这些难以理解的概念。

Open Swift的语言规范可以在这篇文档中描述完备。开发者不需要阅读一部厚厚的Primer，也不需要听谭浩强讲一个学期的编程课程，只需要看懂这篇文档，就可以基本上全面掌握Open Swift的语言规范。然后就可以惬意地将注意力放在创造性地解决问题上，比如如何用更多的数据和很好的算法训练你的神经网络，或者如何更快地抢到红包，或者如何抓到更多的Pokemon，而将啃【C++ Primer】和STL源代码的事情交给我们~~


----------


##1. 基本类型、表达式和运算符

Open Swift的一个变量只能被赋值为6种基本数据类型之一：布尔值、数字、字符串、JSON对象、函数、空。
```js
var a = true;
var b = 6688000;
var c = "welcome";
var d = ["d1" : " string in JSON ", "d2" : 100256];
var e = function (a, b) {
	return a + b;
};
var f = null;
```
布尔值可以取值为`true`或`false`；数字可以是正数、负数、实数、大整数，解释器会对不同数据类型做优化处理；字符串可以由一对或三对单引号或双引号包围。JSON对象是内置的容器，可以模拟列表、队列、堆栈、哈希表的操作。Open Swift的类机制也是基于这个容器实现。语义上，Open Swift的JSON具有如下规则：
>JSON规则：
1. 并列的数据之间用逗号, 分隔。
2. 映射用冒号: 表示。
3. 并列数据的集合（数组）和 映射的集合（对象）用方括号[] 表示。

变量可以被赋值为一个函数，上例中的`e`就是一种匿名函数定义形式。
对于空，JavaScript中有`null` `undefined` `数字0` `空字符串`等形式，它们有时相等有时不等，造成了很大的混乱。为了避免这样的混乱，Open Swift在做比较时，将`null` `数字0` `空字符串` `空JSON对象` `false`都看做是等价形式。

在Open Swift中定义变量必需指定它的生存周期：
```js
var a = 123;
class A{
	var a = 456;
	unique b = "welcome";
}

local uuid;

if(uuid == null){
	uuid = 6553510024 + 42 * ( 10 + 8 );
}else{
	log("uuid is " + uuid);
}
```
在Open Swift中定义变量有三个关键字`var` `local` `unique`可以指定变量的生存周期。
a. `var`表示变量生存在当前变量空间，当前变量空间被销毁时（例如函数返回），该变量所对应内存引用计数减1，若引用计数为0，则回收对应内存资源。（详见：深度篇·内存管理机制）
b. `unique`作用在类机制中，表示该变量所属的对象，在被复制或继承时，不做拷贝，复用原来的变量。（详见：类机制）
c. `local`作用的变量将被持久化到硬盘中，程序下次启动时可以通过同样的变量名进行访问。上面的例子中使用local变量保存一个数字。第一次运行uuid的初始值为空，第二次运行uuid的值为6553510780。

在JavaScript中定义变量可以不写var，变量的生存周期就在global变量空间上。Open Swift的并不采用这样的设计，所有定义的变量必需指定生存周期，否则在任何上下文中将不能使用这个变量。

Open Swift支持多值赋值
```js
var (p1, p2, p3) = (-1111, 2, 6666);
var default_size = [width: 1024, height:760]；
var (x , y , size, message) = (550, true, default_size, "H264");
```
上面一直在使用的= + ==其实是运算符。和很多语言一样，Open Swift使用小括号 ( ) 来指定表达式优先级。而默认的运算符优先级在解释器源代码（C++）里是这样设置的：
```cpp
void OperatorMap::setDeaultPriority() {
	this->addOperatorPriority("! ~ ++ --");
	this->addOperatorPriority("* / %");
	this->addOperatorPriority("+ -");
	this->addOperatorPriority("<< >>");
	this->addOperatorPriority("< > <= >=");
	this->addOperatorPriority("== !=");
	this->addOperatorPriority("& ^ |");
	this->addOperatorPriority("&& ||");
	this->addOperatorPriority("+= -= *= /=");
	this->addOperatorPriority("=");
}
```
Open Swift提供了作用数字和字符串的大部分运算符的默认处理方法。也允许用户自定义重载运算符。例如下面的代码，定义了两个向量做比较时的处理方法：
```js
operator["<", "Vector<Vector"] = function (left, right) {
	if (left == false) {
		return left;
	}
	var result = Vector.compare(left.pre_element, right);
	result.pre_element = right;
	return result;
}
```
上例中，result是一个布尔值，pre_element是布尔值的一个内置变量，可以用来在表达式运算中传递一些数据。


----------


##2. 控制流
和大部分编程语言类似，在Open Swift里，可以使用下面这些关键字来定义代码的逻辑控制流：
```js
if else for in while break continue
```
下面的例子可以完全涵盖了Open Swift的控制流语法：
```js
var colors = ["red", "blue", "green", "black", "white"];
for (var i = 0; i < JSON.getLength(colors); i++) {
	log("we have color " + colors[i]);
}

var i = 0;
while (true) {
	var color = colors[i];
	i++;
	if (color == "red") {
		log("draw red color");
		continue;
	} else if (color == "blue") {
		break;
	}
}

for (var color in colors) {
	log("we also have color " + color);
}
var colorMap = ["red" : "0x0000ff", "blue" : "0xff0000", "green" : "0x00ff00", "black" : "0x000000", "white" : "0xffffff"];
for (var (color, value) in colorMap) {
	log("The color @@'s value is @@." % color % value);
	if (color == "red") {
		log("red color");
	} else if (color == "blue") {
		log("blue color");
	} else (color == "green") {
		log("green color")
	} else {
		log("other color")
	}
}
```
注意：
a. Open Swift并不支持switch case这样的语法形式，因为它可以被语义更明确的if else串替代。
b. else if中的if可以省去
c. 用 for var (key, value) in JSON的方式去遍历JSON时，key可能为空。例如遍历这样的JSON：`[a:1,b:2,"string c"]`


----------
##3. 函数
Open Swift具有如下的函数定义的等价形式：
形式 1：
```js
function showMessage1() {
	log("welcome to China!")
}
```
形式 2：
```js
var showMessage2 = function () {
	log("welcome to Beijing!")
}
```
在运行时，上面两种函数的定义形式是等价的，唯一的区别是，在同一级语义里，第一种函数定义是被解释器优先执行的，而第二种匿名函数定义的主体是一个表达式，和同级语义里的其他表达式一起被顺序执行。这就解释了为什么下面例子中的函数可以在定义之前被调用。

 Open Swift支持定义多值返回函数：
```js
var(x, y, size) = getPoint();

function getPoint() {
	var size = [width : 1024, height : 768];
	var x = 551 / size.width;
	var y = 335 / size.height;
	return (x, y, size)
}
```
类似于JavaScript， Open Swift可以将一个函数类型作为函数的返回值：
```js
function test1() {
	return function (message) {
		log(message + 15535);
	}
}

var f = test1();
f("Hello ");
```
除此之外函数的返回值可以是一个JSON对象。

Open Swift也可以把一个函数作为参数传递给另外一个函数去处理：
```js
 test2(function (message) {
 	log(message);
 }, 100);

 function test2(innerFunction, times) {
 	var message = "inner function is call @@ times";
 	for (int i = 0; i < times; i++) {
 		innerFunction(message % i);
 	}
 }
```
Open Swift函数的参数和返回值可以是Open Swift的6种基本数据类型之一，也可以是它们的组合。

Open Swift在函数内部提供的反射的功能，可以通过`FUNCTION_name` `PARAM` `PARAM_length`这些变量，去访问函数内部的一些特征，例如：
```js
sum(1, 2, 3, 4, 5, 6, 7);
showColor("red", "blue", "green", "black", "white");

function sum() {
	var result = 0;
	for (var number in PARAM) {
		result += number;
	}
	return result;
}

function showColor() {
	for (var i = 0; i < PARAM_length; i++) {
		var color = PARAM[i];
		log("we also color " + color);
	}
	log(FUNCTION_name); // print: showColor
}
```


----------


##4. 类机制
Open Swift具有如下的类定义的等价形式：
形式 1：
```js
A.getID(); //print: this is Class A 8888

class A {
	var message = "this is Class A ";
	var id = 8888;
	function getID() {
		log(this.message + this.id);
	}
}
```
形式 2：
```js
B.getID(); //print: this is Class B 7777

class B {
	var message = "this is CLASS B ";
	var id = 7777;
	var getID = function () {
		log(this.message + this.id);
	}
}
```
形式 3：
```js
var C = [
	message : "this is JSON C ",
	id : 6666,
];
C.getID = function () {
	log(this.message + this.id);
}
C.getID(); //print: this is JSON C 6666
```
形式 4：
```js
var D = [
	message : "this is JSON D ",
	id : 5555,
	getID : function () {
		log(this.message + this.id);
	}
]

D.getID(); //print: this is JSON D 5555
```
形式 5：
```js
var e = new E();
e.getID = function () {
	log(this.message + this.id);
}
e.getID(); //print: this is Class E 2222

class E {
	var message = "this is Class E ";
	var id = 2222;
}
```

在运行时，上面五种类的定义形式是等价的，那是因为在Open Swift里，类和实例都被理解为一个JSON对象，而JSON对象的核心就是Open Swift的6种基本数据类型的一个容器。和函数定义一样，在同一级语义里，类的定义是被解释器优先执行的。

在Open Swift中， 类的继承和new 实例的实质都是对JSON对象的克隆：
```js
class A {
	var message = "This is A";
	function getMessage() {
		log(message);
	}
}

class B extends A {
	var message = "This is B ";
	var id = 123564;
	function getID() {
		log(message + id);
	}
}

A.getMessage(); //print: This is A
B.getID(); //print: This is B 123564
var b = new B();
b.message = "This is b";
b.getMessage(); //print: This is b
```
而JSON对象在克隆时，对于function函数类型元素和由关键字unique指定的元素量不做内存拷贝，复用的唯一一个对象：
```js
class A{
	var id = 456;
	unique message = "Welcome to China!";
    function getMessage() {
        log(message);
    }
}
A.getMessage(); //print: Welcome to China!
var a1 = new A();
a1.message = "Welcome to Beijing!"
A.getMessage(); //print: Welcome to Beijing!
a1.getMessage(); //print: Welcome to Beijing!

var a2 = new A();
a1.message = "Welcome to ShangHai!"
A.getMessage(); //print: Welcome to ShangHai!
a1.getMessage(); //print: Welcome to ShangHai!
a2.getMessage(); //print: Welcome to ShangHai!
```
类机制中this关键字指定变量按照运行时的类关系去查找，否则就按照变量所处的语义闭包关系去查找。（详见：进阶篇·回调和闭包）。这就是下面例子中两个message会有不同结果的原因：
```js
test1();
function test1() {
	var message = "Welcome to Beijing!";
	A.getMessage = function () {
		log(message); //print: Welcome to Beijing!
		log(this.message); //print: Welcome to China!
	}
	A.getMessage();
	class A {
		var message = "Welcome to China!";
	}
}
```
Open Swift提供类的反射机制，可以通过`CLASS_name` `CLASS_parent_name ` `FIELD_length`这些变量，去访问类（实例、JSON对象）的一些特征，例如：
```js
class A {
	var message = "This is A";
	function getMessage() {
		log(message);
	}
}

class B extends A {
	var message = "This is B ";
	var id = 123564;
	function getID() {
		log(CLASS_name); //print: B
		log(CLASS_parent_name); //print: A
		log(FIELD_length); //print: 4
	}
}
```
Open Swift的语言规范描述完备。

----
![](http://upload-images.jianshu.io/upload_images/2422165-11c80f67e81770a9.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)


  [1]: http://static.zybuluo.com/wsds/a075da0re01pdd32503v0q9n/image_1aodbn9pd12b613dlki4tl8194o9.png
  [2]: http://static.zybuluo.com/wsds/0fbuqdaf1tfnjmwec7r9k0p2/image_1aodbphlqm7l2j3vah1tcl1im5m.png