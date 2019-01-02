"use strict";
let randomhash = require("../build/Release/randomhash");
let fs = require("fs");
let path = require("path");
let lineReader = require("readline");

let testsFailed = 0, testsPassed = 0;
let lr = lineReader.createInterface({
    input: fs.createReadStream(path.join(__dirname, "randomhash.txt"))
});
lr.on("line", function (line) {
    let line_data = line.split(" ");
    let result = randomhash.hashSync(Buffer.from(line_data[0], "hex"));
    console.log(result.toString("hex"));
    if (line_data[1] !== result.toString("hex")) {
        testsFailed += 1;
    } else {
        testsPassed += 1;
    }
});
lr.on("close", function(){
    if (testsFailed > 0){
        console.log(testsFailed + "/" + (testsPassed + testsFailed) + " tests failed");
    } else {
        console.log(testsPassed + " tests passed");
    }
});
