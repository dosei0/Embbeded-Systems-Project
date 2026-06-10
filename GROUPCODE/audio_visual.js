var level;
var x;
var y;
var z;
var micLevel;
var biggest = 500;
var count = 0;
var frequency;
var frequency2;
var graph;
var graph2;
var bass;
var average;
var threshold = 120;
var time;
var flag;
var samples = 0;
var tick;
var temp1 = 0;
var timeOut = 0;
var newMic;
var squares = new Array(100);
var waveform = new Array(25);
var wait = 3;
var stayOn;
var colourLimit
var colours
var xStart
var yStart
var xEnd
var yEnd
var distance = 0
var lines = new Array(62)
var rotation = 0

// for (x = 0; x < 4; x++) {
//   entry[x] = 0;
// }
//
// for (x = 0; x < 62; x++) {
//   lines[x] = entry;
// }

for (x = 0; x < 100; x++) {
  squares[x] = 0;
}


function setup() {
  createCanvas(1500, 1000);
  level = new p5.AudioIn();
  level.start();
  frequency = new p5.FFT(0.8, 512);
  frequency.setInput(level);
  frequency2 = new p5.FFT();
  frequency2.setInput(level);
  bouncingSetup();
}
function draw() {

	background(51);


  micLevel = level.getLevel();
  micLevel = micLevel * 1000;
  micLevel = Math.round(micLevel);

  //compute FFT graph
  graph = frequency.analyze();
  graph2 = frequency2.analyze();

  //draw graph
  for (z = 0; z < 85; z++) {
    let c = color(z * 3 + 40, 0, 0);
    fill(c);
    rect(z * 6 + 500, 500, 3, -graph[z]);
  }

  //call pulsing circles
  circles();
  timeOut--;

  //beat detection
  micLevel = (micLevel / 200) * 2500;
  if (micLevel > biggest - 100) {
    let c = color(255, 0, 0);
    fill(c);
    noStroke();
    c = color(128, 0, 0);
    fill(c);

    if (timeOut < 1) {
      if (micLevel > 200) {
        let c = color(255, 0, 0);
        fill(c);
        circle(100, 100, average / 4);
        circle(450, 450, average / 4);
        circle(450, 100, average / 4);
        circle(100, 450, average / 4);
        stayOn = 7;
        timeOut = 20;
      }
    }
  }

  if (stayOn > 0) {
    let c = color(36 * stayOn + 50, 0, 0);
    fill(c);
    circle(100, 100, average / 4);
    circle(450, 450, average / 4);
    circle(450, 100, average / 4);
    circle(100, 450, average / 4);
    stayOn--;
  }

  newMic = micLevel / 40;
  let c = color(0, 255, 0);
  fill(c);
  for (x = 0; x < 50; x++) {
    if (x * y > micLevel) {
      break;
    }
    stroke(0);
    let c = color(x * 5, 100, 50);
    fill(c);
    for (y = 0; y < 10; y++) {
      rect(y * 32 + 500, x * 8 - 10 + 520, 25, 5);
      if (x * y > newMic) {
        break;
      }
    }
  }

	// colourLimit = micLevel / 50
  // c = color(0, 255, 0);
  // fill(c);
  // stroke(0);
	// print(colourLimit)
  // for (x = 0; x < 10; x++) {
  //   for (y = 0; y < 10; y++) {
  //     let lemon = x * 10 + y;
  //       squares[lemon] + 1
  //       square(y * 40, 500 + x * 40, 30);
	//
  //     if (x * y > colourLimit) {
  //       break;
  //     }
  //   }
	//
  //   if (x * y > colourLimit) {
  //     break;
  //   }
  //



		strokeWeight(4);
		if (distance > 6.2){
			distance = 0
		}
		distance = distance + 0.1
		xStart = 250 + (70 * Math.cos(distance))
		yStart = 750 + (70 * Math.sin(distance))

		xEnd = 250 + (70 * (newMic / 20) * Math.cos(distance))
		yEnd = 750 + (70 * (newMic / 20) * Math.sin(distance))

		let entry = new Array(4)
		entry[0] = xStart
		entry[1] = yStart
		entry[2] = xEnd
		entry[3] = yEnd


		lines[rotation] = entry

		rotation++
		if(rotation > 61){
			rotation = 0
		}

		for(x=0; x < 62; x++){
			if(lines[x] !== undefined){
				line(lines[x][0],lines[x][1],lines[x][2],lines[x][3])
			}
		}
		circle(250,750,70)

		strokeWeight(1);





  if (micLevel > biggest) {
    biggest = micLevel;
  }

  count++;

  if (count > 20) {
    biggest = biggest - 200;
    count = 0;
  }
}

function circles() {
  c = color(255, 0, 255);
  fill(c);

  for (tick = 0; tick < 100; tick++) {}

  c = color(0, 0, 125);
  fill(c);
  average = 0;

  for (z = 0; z < 50; z++) {
    average = average + graph2[z];
  }

  average = average / 50;

  if (average > threshold) {
    flag = true;
    threshold = average;
  }

  if (average >= threshold - 50 || time > 0) {
    time--;
    if (flag == true) {
      time = 10;
    }
    flag = false;

    circle(275, 275, average / 1.3);
  }

  samples++;

  if (samples > 200) {
    samples = 0;
    threshold = threshold - 20;
  }

  wait--;

  for (x = 0; x < 24; x++) {
    circle(1500 - waveform[x].centre - 80, x * 40 + 40, 15);
  }

  if (wait < 1) {
    waveform[0].centre = micLevel;
    bounce();

    for (x = 0; x < 24; x++) {
      circle(1500 - waveform[x].centre - 80, x * 40 + 40, 15);
    }
    wait = 3;
  }
}

function sleep(milliseconds) {
  var start = new Date().getTime();
  for (var i = 0; i < 1e7; i++) {
    if (new Date().getTime() - start > milliseconds) {
      break;
    }
  }
}

function bouncingSetup() {
  for (x = 0; x < 25; x++) {
    waveform[x] = new ball();
    circle(1450, x * 40 + 40, 15);
  }
}

function bounce() {
  for (x = 24; x > 0; x--) {
    waveform[x].centre = waveform[x - 1].centre;
  }
}

function ball() {
  this.centre = 20;
}
