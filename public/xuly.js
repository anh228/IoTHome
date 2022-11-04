var socket = io("http://localhost:3300");
socket.on("server-update-data", function (data) {
  // Home page
  $("#currentTemp").html(data.temp);
  $("#currentHumi").html(data.humi);
  $("#currentSolid").html(data.solid);
  $("#currentLight").html(data.light);

  // Warning mode
  // var warningSection = document.getElementById("warningSection")
  // if (data.temp > 40 || data.solid < 55) {
  //     warningSection.classList.add("warning-mode-on")
  // } else {
  //     warningSection.classList.remove("warning-mode-on")
  // }
  //History page
  $("#id-content").append("<div class='h-para'>" + data.id + "</div>");
  $("#time-content").append("<div class='h-para'>" + data.time + "</div>");
  $("#temp-content").append("<div class='h-para'>" + data.temp + "</div>");
  $("#humi-content").append("<div class='h-para'>" + data.humi + "</div>");
  $("#solid-content").append("<div class='h-para'>" + data.solid + "</div>");
  $("#light-content").append("<div class='h-para'>" + data.light + "</div>");
});

socket.on("send-full", function (data) {
  // History page
  $("#light-content").html("");
  $("#time-content").html("");
  $("#temp-content").html("");
  $("#humi-content").html("");
  $("#solid-content").html("");
  $("#id-content").html("");

  // console.log(data)
  data.forEach(function (item) {
    $("#light-content").append("<div class='h-para'>" + item.light + "</div>");
    $("#time-content").append("<div class='h-para'>" + item.time + "</div>");
    $("#temp-content").append("<div class='h-para'>" + item.temp + "</div>");
    $("#humi-content").append("<div class='h-para'>" + item.humi + "</div>");
    $("#solid-content").append("<div class='h-para'>" + item.solid + "</div>");
    $("#id-content").append("<div class='h-para'>" + item.id + "</div>");
  });
});

// ---- Control devices ----

function parkHoca() {
  var checkBox = document.getElementById("parkHoca");
  if (checkBox.checked == true) {
    // alert('LED On')
    socket.emit("parkHocaChange", "on");
  } else {
    // alert('LED Off')
    socket.emit("parkHocaChange", "off");
  }
}

function parkLamp1() {
  var checkBox = document.getElementById("parkLamp1");
  if (checkBox.checked == true) {
    // alert('LED On')
    socket.emit("parkLamp1Change", "on");
  } else {
    //alert('LED Off')
    socket.emit("parkLamp1Change", "off");
  }
}

function parkDoor() {
  var checkBox = document.getElementById("parkDoor");
  if (checkBox.checked == true) {
    //alert('LED On')
    socket.emit("parkDoorChange", "on");
  } else {
    // alert('LED Off')
    socket.emit("parkDoorChange", "off");
  }
}

function parkLight2() {
  var checkBox = document.getElementById("parkLight2");
  if (checkBox.checked == true) {
    // alert("LED On");
    socket.emit("parkLight2Change", "on");
  } else {
    //alert("LED Off");
    socket.emit("parkLight2Change", "off");
  }
}

function parkFan() {
  var checkBox = document.getElementById("parkFan");
  if (checkBox.checked == true) {
    // alert("LED On");
    socket.emit("parkFanChange", "on");
  } else {
    //  alert("LED Off");
    socket.emit("parkFanChange", "off");
  }
}

// ---- RTC ----
