// Simple program for getting data.
// Created by Philip R. Simonson.
var value = "";
var year = 0;
var company = "";
var quit = "";
var done = 0;

done = 1;
while (done) {
	value = input("Enter your name");
	year = input("Enter your birth date");
	company = input("Enter your company");
	year = 2024 - year;
	print "Hello, " + value + " you work at " + company + " and are ", year , " old.";

	quit = input("Do you want to quit (Y/n)");
	if (quit == "") { quit = "y"; }
	if (quit == "y") { done = 0; }
}
