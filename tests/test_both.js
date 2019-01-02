"use strict";
let randomhash = require("../build/Release/randomhash");
let fs = require("fs");
let path = require("path");
let lineReader = require("readline");

let testsFailed = 0, testsPassed = 0, line_count=0;
let lr = lineReader.createInterface({
    input: fs.createReadStream(path.join(__dirname, "randomhash.txt"))
});
lr.on("line", function (line) {
    let line_data = line.split(" ");
    line_count += 1;
    randomhash.hashAsync(Buffer.from(line_data[0], "hex"), function(err, result_async){
	console.log(result_async.toString("hex"));
        if (line_data[1] !== result_async.toString("hex")){
            testsFailed += 1;
        } else {
            testsPassed += 1;
        }
	let result_sync = randomhash.hashSync(Buffer.from(line_data[0], "hex"));
        if (result_async.toString("hex") !== result_sync.toString("hex")){
            console.log("The two functions do not agree");
        }
        if (line_count === (testsFailed + testsPassed)){
            if (testsFailed > 0){
                console.log(testsFailed + "/" + (testsPassed + testsFailed) + " tests failed");
            } else {
                console.log(testsPassed + " tests passed");
            }
        }
    });
});
