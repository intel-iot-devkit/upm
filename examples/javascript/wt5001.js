/*
* Author: Zion Orent <zorent@ics.com>
* Copyright (c) 2015 Intel Corporation.
*
* This program and the accompanying materials are made available under the
* terms of the The MIT License which is available at
* https://opensource.org/licenses/MIT.
*
* SPDX-License-Identifier: MIT
*/

var MP3Player = require('jsupm_wt5001');

function printUsage(progname)
{
	console.log("Usage: node " + progname + " <command>");
	console.log("Commands:");
	console.log("0  - stop playing");
	console.log("1  - start playing track 1");
	console.log("2  - pause/un-pause playback");
	console.log("3  - next track");
	console.log("4  - previous track");
}

// Instantiate a WT5001 serial MP3 player on uart 0.
// This example was tested on the Grove Serial MP3 module.

var myMP3Player = new MP3Player.WT5001(0);

var cmd = -1;
if (process.argv.length > 2)
	cmd = parseInt(process.argv[2]);

if (!myMP3Player.setupTty(MP3Player.int_B9600))
{
	console.log("Failed to setup tty port parameters");
	process.exit(0);
}

switch (cmd)
{
	case 0:
		myMP3Player.stop();
		break;

	case 1:
		myMP3Player.play(MP3Player.WT5001.SD, 1);
		break;

	case 2:
		myMP3Player.pause();
		break;

	case 3:
		myMP3Player.next();
		break;

	case 4:
		myMP3Player.previous();
		break;

	default:
		// nothing, just output usage, and info below
		printUsage(process.argv[1]);
		break;
}

// print out some information
var vol = new MP3Player.uint8Array(0);
myMP3Player.getVolume(vol);
console.log("The current volume is: " + vol.getitem(0));

var ps = new MP3Player.uint8Array(0);
myMP3Player.getPlayState(ps);
console.log("The current play state is: " + ps.getitem(0));

var numf = new MP3Player.uint16Array(0);
myMP3Player.getNumFiles(MP3Player.WT5001.SD, numf);
console.log("The number of files on the SD card is: " + numf.getitem(0));

var curf = new MP3Player.uint16Array(0);
myMP3Player.getCurrentFile(curf);
console.log("The current file is: " + curf.getitem(0));

// set the date
myMP3Player.setDate(2015, 3, 14);

// set the time
myMP3Player.setTime(9, 26, 53);

var year = new MP3Player.uint16Array(0);
var month = new MP3Player.uint8Array(0);
var day = new MP3Player.uint8Array(0);
myMP3Player.getDate(year, month, day);
var mp3date = month.getitem(0) + "/";
mp3date += (day.getitem(0) + "/");
mp3date += year.getitem(0);
console.log("The device date is: " + mp3date);

var hour = new MP3Player.uint8Array(0);
var minute = new MP3Player.uint8Array(0);
var second = new MP3Player.uint8Array(0);
myMP3Player.getTime(hour, minute, second);
var mp3time = hour.getitem(0) + ":";
mp3time += (minute.getitem(0) + ":");
mp3time += second.getitem(0);
console.log("The device time is: " + mp3time);

// Print message when exiting
process.on('SIGINT', function()
{
	console.log("Exiting...");
	process.exit(0);
});
