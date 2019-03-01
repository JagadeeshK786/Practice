KeyWords.onkeypress = (e) => {
    if (e.keyCode == 13) {
        var keyWord = document.getElementById('KeyWords').value;
        if (!isEmptyOrSpaces(keyWord))
            doSearch(keyWord);
    }
};

function isEmptyOrSpaces(str) {
    return str === null || str.match(/^ *$/) !== null;
}

function doSearch(keyWord) {
    var look = encodeURI('http://www.zdic.net/sousuo?lb_a=hp&lb_b=mh&lb_c=mh&tp=tp1&q=' + keyWord);
    window.open(look, '_blank');
}
