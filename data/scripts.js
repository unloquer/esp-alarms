function time_readable(dow, hours, minutes,triegohor,triegomin) {
	var dow_readable = {
		1 : "Domingo",
		2 : "Lunes",
		3 : "Martes",
		4 : "Miercoles",
		5 : "Jueves",
		6 : "Viernes",
		7 : "Sabado",
	}[dow];

	hours = ("0" + hours).slice(-2);
	minutes = ("0" + minutes).slice(-2);
	triegomin = ("0" + triegomin).slice(-2);

	return dow_readable + ", " + hours + ":" + minutes + ", " + triegomin;
}

function alarmlist_update() {
	$.get("alarms", function (res) {
		const alarms = res.split(';');

		alarms.pop();
		alarms.pop();

		$("#alarmlist").html("");
		alarms.forEach(function (d) {
			const data = d.split(',');
			const alarm = {
				id: data[0],
				dow: data[2],
				hrs: data[3],
				min: data[4],
				trm: data[5]
			};
			$("#alarmlist").append($("<tr>")
				.append($("<td>").addClass("alarm_id").text(alarm.id))
				.append($("<td>").addClass("alarm_time").text(time_readable(alarm.dow, alarm.hrs, alarm.min, alarm.trm)))
				.append($("<td>").append($("<a>").addClass("alarm_delete").data("alarmid", alarm.id).html("&#215;")))
			);
		});

		if (alarms.length === 0) {
			$("#alarmlist").append($("<tr>").addClass("empty").append($("<th>").text("No alarms set")));
		}

		$(".alarm_delete").click(function () {
			$.get("waketime_del", { id : $(this).data("alarmid") }, function (res) {
				if (res !== "ok") alert("Error: " + res);
				alarmlist_update();
			});
		});
	});
}

function setTime() {
	const d = new Date();
	const time = `h=${d.getHours()}&m=${d.getMinutes()}&s=${d.getSeconds()}&MM=${d.getMonth()+1}&DD=${d.getDate()}&YY=${d.getFullYear()-2000}`;
	$.get(`time?${time}`, (res) => {
	})
}

$(function () {
	$('#intensitybuttons input[type="button"]').each(function (idx, button) {
		$(button).css("background-color", "rgba(0, 150, 255, " + $(button).data("color") / 100 + ")");
	});

	$('#intensitybuttons input[type="button"]').click(function () {
		$.get("intensity_set", { intensity : $(this).data("intensity") }, function (res) {
			if (res !== "ok") alert("Error: " + res);
		});
	});

	if ($("#alarmlist").length) alarmlist_update();

	setTime();

	$("#add").click(function () {
		var dow = $("#dow").val();
		var hour = parseInt($("#hours").val());
		var min = parseInt($("#minutes").val());
		var interval = parseInt($("#triegomin").val());

		if (isNaN(hour) || hour >= 24 || isNaN(min) || min >= 60) {
			alert("Invalid time!");
		} else {
			console.log(`alarm?dow=${dow}&hour=${hour}&min=${min}&interval=${interval}`);
			$.get(`alarm?dow=${dow}&hour=${hour}&min=${min}&interval=${interval}`, function (res) {
				alarmlist_update();
			});
		}
	});
});
