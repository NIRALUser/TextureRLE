var Module = require('./ComponentSizeRLE.js');
var path = require('path');

var inputImage = "/Users/prieto/NetBeansProjects/UNC/data/canine/atlas_DD_039_t1w.nrrd";
var inputMask = "/Users/prieto/NetBeansProjects/UNC/data/canine/DD_039_seg.nii.gz";
var outputRLEVar = "outputRLEVar";
var outputHistogramVar = "outputHistogramVar";

var MountDirectory = function(dir){    
    var dirbuf = Module._malloc(dir.length+1);
    Module.writeStringToMemory(dir, dirbuf);
    Module._MountDirectory(dirbuf);
}

var GetStringBuffer = function(str){
    var strbuf = Module._malloc(str.length+1);
    Module.writeStringToMemory(str, strbuf);
    return strbuf
}

var executeRLE = function(argvector){

	function pad(argv) {
	    for (var i = 0; i < 3; i++) {
	      argv.push(0);
	    }
	}

	var argv = [Module.allocate(Module.intArrayFromString('rle'), 'i8', Module.ALLOC_NORMAL) ];
  	pad(argv);
  	for(var i = 0; i < argvector.length; i++){
  		argv.push(Module.allocate(Module.intArrayFromString(argvector[i]), 'i8', Module.ALLOC_NORMAL));
  		pad(argv);
  	}
  	argv.push(0);
  	argv = Module.allocate(argv, 'i32', Module.ALLOC_NORMAL);

  	var argc = argvector.length + 1;

  	return Module._executeRLE(argc, argv);
}

// Source: http://www.bennadel.com/blog/1504-Ask-Ben-Parsing-CSV-Strings-With-Javascript-Exec-Regular-Expression-Command.htm
// This will parse a delimited string into an array of
// arrays. The default delimiter is the comma, but this
// can be overriden in the second argument.

function CSVToArray(strData, strDelimiter) {
    // Check to see if the delimiter is defined. If not,
    // then default to comma.
    strDelimiter = (strDelimiter || ",");
    // Create a regular expression to parse the CSV values.
    var objPattern = new RegExp((
    // Delimiters.
    "(\\" + strDelimiter + "|\\r?\\n|\\r|^)" +
    // Quoted fields.
    "(?:\"([^\"]*(?:\"\"[^\"]*)*)\"|" +
    // Standard fields.
    "([^\"\\" + strDelimiter + "\\r\\n]*))"), "gi");
    // Create an array to hold our data. Give the array
    // a default empty first row.
    var arrData = [[]];
    // Create an array to hold our individual pattern
    // matching groups.
    var arrMatches = null;
    // Keep looping over the regular expression matches
    // until we can no longer find a match.
    while (arrMatches = objPattern.exec(strData)) {
        // Get the delimiter that was found.
        var strMatchedDelimiter = arrMatches[1];
        // Check to see if the given delimiter has a length
        // (is not the start of string) and if it matches
        // field delimiter. If id does not, then we know
        // that this delimiter is a row delimiter.
        if (strMatchedDelimiter.length && (strMatchedDelimiter != strDelimiter)) {
            // Since we have reached a new row of data,
            // add an empty row to our data array.
            arrData.push([]);
        }
        // Now that we have our delimiter out of the way,
        // let's check to see which kind of value we
        // captured (quoted or unquoted).
        if (arrMatches[2]) {
            // We found a quoted value. When we capture
            // this value, unescape any double quotes.
            var strMatchedValue = arrMatches[2].replace(
            new RegExp("\"\"", "g"), "\"");
        } else {
            // We found a non-quoted value.
            var strMatchedValue = arrMatches[3];
        }
        // Now that we have our value string, let's add
        // it to the data array.
        arrData[arrData.length - 1].push(strMatchedValue);
    }
    // Return the parsed data.
    return (arrData);
}

function CSV2JSON(csv, strDelimiter) {
    var array = CSVToArray(csv, strDelimiter);
    var objArray = [];
    for (var i = 1; i < array.length; i++) {
        objArray[i - 1] = {};
        for (var k = 0; k < array[0].length && k < array[i].length; k++) {
            var key = array[0][k];
            objArray[i - 1][key] = array[i][k]
        }
    }

    var json = JSON.stringify(objArray);
    var str = json.replace(/},/g, "},\r\n");

    return str;
}

MountDirectory(path.dirname(inputImage));
MountDirectory(path.dirname(inputMask));


var argvector = ["--i", inputImage, "--l", inputMask, "--outputRLEVar", GetStringBuffer(outputRLEVar).toString(), "--outputHistogramVar",  GetStringBuffer(outputHistogramVar).toString()];


if(executeRLE(argvector) == 0){
	console.log(CSV2JSON(Module.variable[outputRLEVar]));
    var outhisto = Module.variable[outputHistogramVar];

	var strarray = outhisto.split('\t');

	for(var i = 0; i < strarray.length; i++){
		var str = strarray[i];
		if(str[0] === '\n' && str[str.length - 1] == '\n'){
			str = str.slice(1, str.length - 2);
		}else if(str[0] === '\n'){
			str = str.slice(1, str.length - 1);
		}else if(str[str.length - 1] == '\n'){
			str = str.slice(0, str.length - 2);
		}
		console.log(CSV2JSON(str));
	}
}