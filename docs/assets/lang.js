// Перемикач мови сторінок (uk/en): ?lang=…, збережений вибір, мова браузера. EN: language switch for the pages.
(function () {
  var pick = function (l) {
    document.documentElement.lang = l;
    try { localStorage.setItem('icua-lang', l); } catch (e) {}
    var b = document.querySelectorAll('.lang button');
    for (var i = 0; i < b.length; i++) b[i].setAttribute('aria-pressed', b[i].getAttribute('data-set') === l ? 'true' : 'false');
  };
  var q = (location.search.match(/[?&]lang=(uk|en)/) || [])[1];
  var saved = null;
  try { saved = localStorage.getItem('icua-lang'); } catch (e) {}
  var nav = (navigator.language || 'en').toLowerCase();
  pick(q || saved || (/^(uk|ru|be)/.test(nav) ? 'uk' : 'en'));
  document.addEventListener('click', function (e) {
    var t = e.target.closest && e.target.closest('.lang button');
    if (t) pick(t.getAttribute('data-set'));
  });
})();
