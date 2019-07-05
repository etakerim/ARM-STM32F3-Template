var sender = document.getElementById("sender");
var loading = document.getElementById("loading");

const ajax_url = "/message"; //form.action;


function current_message() {
    var xhttp = new XMLHttpRequest();
    xhttp.open("GET", ajax_url, true);
    xhttp.send();

    xhttp.addEventListener("load", function(event) {
        console.log(this.responseText)
        var response = JSON.parse(this.responseText);
        if (this.status == 200) {
            var lastmsg = document.getElementById("lastmsg");
            lastmsg.innerHTML = response.message;
        }
    });
}


// https://code.lengstorf.com/get-form-values-as-json/
function form_to_json(elements) {
    return [].reduce.call(elements, (data, element) => {
        if (element.type != "submit") {
            data[element.name] = element.value;
        }
        return data;
    }, {});
}

sender.addEventListener("submit", function(event) {
    event.preventDefault();

    var status = document.getElementById("status")
    status.innerHTML = "";
    loading.style.display = "";
    var xhttp = new XMLHttpRequest();
    const form = event.target;

    var msg = form_to_json(form.elements);
    if (!msg.message) {
        return
    }

    msg.glyphs = {}
    for (var i = 0; i < loaded_glyphs.length; i++) {
        if (loaded_glyphs[i] !== undefined)
            msg.glyphs[i] = glyph_binary(loaded_glyphs[i]);
    }

    const json = JSON.stringify(msg);

    xhttp.addEventListener("load", function(event) {
        var response = JSON.parse(this.responseText);
        var t = "";
        if (this.status == 200) {
            t = "Správa bola úspešne prijatá";
            current_message();
        } else if (this.status == 503) {
            t = "Chyba služby - " + response.error;
        }

        status.style.display = "";
        status.innerHTML = ("<b>Odpoveď</b>: (" + this.status + ") - " + t);
        loading.style.display = "none";
    });

    xhttp.open("POST", ajax_url, true);
    xhttp.setRequestHeader("Content-Type", "application/json");
    xhttp.send(json);
});

window.addEventListener('load', current_message);
