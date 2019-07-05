var canvas = document.getElementById("glyph_design");
var deletebtn = document.getElementById("clear_design");
var downloadbtn = document.getElementById("download_design");
var uploadbtn = document.getElementById("upload_design");
var loadbtn = document.getElementById("save_design");
var memchars = document.getElementsByClassName("memcanvas");
var memo = document.getElementById("message");

const glyph_w = 5;
const glyph_h = 8;
canvas.width = 100;
canvas.height = canvas.width * (glyph_h / glyph_w);

var loaded_glyphs = Array(8);
var glyph = glyph_clear();

glyph_editor();
load_custom_glyphs();

function load_custom_glyphs() {
    for (var i = 0; i < 8; i++) {
        var c = document.getElementById("memchar" + i);
        c.width = glyph_w * 5
        c.height = glyph_w * 8

        var ctx = c.getContext("2d");
        ctx.fillStyle = "#000000";
        ctx.fillRect(0, 0, canvas.width, canvas.height);
    }
}

function glyph_clear() {
    return Array(glyph_h).fill(0).map(x => Array(glyph_w).fill(false));
}

function glyph_binary(g) {
    var bitmap = []

    for (var i = 0; i < glyph_h; i++) {
        var b = 0;
        for (var j = 0; j < glyph_w; j++) {
            if (g[i][j]) {
                b |= (1 << j);
            }
        }
        bitmap.push(b);
    }
    return bitmap
}

function glyph_from_image(img) {
    if (img.data.length != 4 * glyph_w * glyph_h)
        return;

    glyph = glyph_clear();
    for (var i = 0; i < img.data.length; i += 4) {
        x = Math.floor((i % (4 * glyph_w)) / 4);
        y = Math.floor(i / (4 * glyph_w));

        if (img.data[i] == 255)
            glyph[y][x] = true;
        else
            glyph[y][x] = false;
    }

}

function glyph_bitmap() {
    var bitmap = document.createElement('canvas');
    bitmap.width = glyph_w;
    bitmap.height = glyph_h;
    glyph_render(bitmap, "white", "black", false);
    return bitmap.toDataURL("image/png");
}

function glyph_editor() {
    glyph_render(canvas, "#ffffff", "#1c81a5", true);
}

function glyph_render(graphics, fg, bg, grid) {
    var ctx = graphics.getContext("2d");
    var square_x = graphics.width / glyph_w;
    var square_y = graphics.height / glyph_h;

    ctx.fillStyle = bg;
    ctx.fillRect(0, 0, graphics.width, graphics.height);

    ctx.fillStyle = fg;
    for (var i = 0; i < glyph_h; i++) {
        for (var j = 0; j < glyph_w; j++) {
            if (glyph[i][j]) {
                ctx.fillRect(j * square_x, i * square_y, square_x, square_y);
            }
        }
    }

    if (!grid)
        return

    ctx.strokeStyle = "#dbdbdb";
    for (var x = 0; x < canvas.width; x += square_x) {
        ctx.beginPath();
        ctx.moveTo(x, 0);
        ctx.lineTo(x, canvas.height);
        ctx.stroke();
    }

    for (var y = 0; y < canvas.height; y += square_y) {
        ctx.beginPath();
        ctx.moveTo(0, y);
        ctx.lineTo(canvas.width, y);
        ctx.stroke();
    }
}

canvas.addEventListener('click', function(event) {
    const sx = canvas.width / glyph_w;
    const sy = canvas.height / glyph_h;

    s = Math.trunc(event.offsetX / sx);
    r = Math.trunc(event.offsetY / sy);
    glyph[r][s] = !glyph[r][s];
    glyph_editor();
});

deletebtn.addEventListener('click', function(event) {
    glyph = glyph_clear();
    glyph_editor();
});

downloadbtn.addEventListener('click', function(event) {
    var dlink = document.createElement('a');
    dlink.href = glyph_bitmap();
    dlink.download = "glyph.png";
    dlink.dataset.downloadurl = ["image/png", dlink.download, dlink.href].join(':');

    document.body.appendChild(dlink);
    dlink.click();
    document.body.removeChild(dlink);
});

uploadbtn.addEventListener('change', function(event) {
    var file = event.target.files[0];
    if (!file)
        return;

    var reader = new FileReader();
    reader.onload = function(e) {
        var uri = e.target.result;
        var img = new Image();
        img.src = uri;

        img.addEventListener('load', function() {
            var c = document.createElement('canvas')
            c.width = img.width;
            c.height = img.height;
            ctx = c.getContext('2d');
            ctx.drawImage(img, 0, 0, c.width, c.height);
            raw = ctx.getImageData(0, 0, img.width, img.height);
            glyph_from_image(raw);
            glyph_editor();
        });
    };

    reader.readAsDataURL(file);
});

loadbtn.addEventListener('click', function(event) {
    var mem = document.getElementById("mem_design").value;
    if (mem >= 0 && mem < loaded_glyphs.length) {
        var c = document.getElementById("memchar" + mem);
        loaded_glyphs[mem] = glyph_clear()
        glyph_render(c, "#ffffff", "#000000", false);

        for (var i = 0; i < glyph_h; i++) {
            for (var j = 0; j < glyph_w; j++) {
                loaded_glyphs[mem][i][j] = glyph[i][j];
            }
        }
    }
});

function add_memchar(event) {
    const memloc = event.target.parentNode.cellIndex;
    const escsym = "\\"+ memloc

    if (memo.selectionStart || memo.selectionStart == '0') {
        var start = memo.selectionStart;
        var end = memo.selectionEnd;
        memo.value = memo.value.substring(0, start)
                    + escsym
                    + memo.value.substring(end, memo.value.length);
    } else {
        memo.value += escsym;
    }
}

for (var i = 0; i < memchars.length; i++) {
    memchars[i].addEventListener("click", add_memchar);
}
