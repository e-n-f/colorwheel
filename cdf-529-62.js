var ss = require('simple-statistics');

for (i = 400; i < 700; i++) {
  console.log(i + " " + (.5 + .5 * (ss.erf((i - 529.174) / (61.959 * Math.sqrt(2))))))
}
