var gulp = require('gulp'),
    run = require('gulp-run'),
    livereload = require('gulp-livereload');

var options = "-T %USERPROFILE%\\asciidoctor-backends -E slim";

gulp.task('build', function(){
  return gulp.src(['*.asciidoc'], {read:false})
    .pipe(run(['asciidoctor',options,'<%= file.path %>'].join(' ')))
    .on('end', livereload.changed);
});

gulp.task('watch', function(){
  livereload.listen();
  gulp.watch(['*.asciidoc', '*.conf', '*-docinfo.*'], ['build']);
});

gulp.task('default', ['build', 'watch']);
