var gulp = require('gulp'),
    livereload = require('gulp-livereload'),
    run = require('gulp-run');

var options = "--verbose -T %USERPROFILE%\\asciidoctor-backends -E slim";

gulp.task('build', function(){
  return gulp.src(['*.asciidoc'], {read:false})
    .pipe(run(['asciidoctor',options,'<%= file.path %>'].join(' ')))
    .on('end', livereload.changed);
});

gulp.task('default', ['build'], function(){
  livereload.listen();
  gulp.watch(['*.asciidoc', '*.conf', '*-docinfo.*'], ['build']);
});
