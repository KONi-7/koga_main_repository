const stickySections = [...document.querySelectorAll('.boxbox')]

window.addEventListener('scroll', (e) => {
  for(let i = 0; i < stickySections.length; i++){
    transform(stickySections[i])
  }
})

function transform(section) {

  const offsetTop = section.parentElement.offsetTop;

  const scrollSection = section.querySelector('.box')

  let percentage = ((window.scrollY - offsetTop) / window.innerHeight) * 100;

  percentage = percentage < 0 ? 0 : percentage > 800 ? 800 : percentage;    //(page-1)*100

  scrollSection.style.transform = `translate3d(${-(percentage)}vw, 0, 0)`
}

//animation
$('.Trigger').on('inview', function(event, isInView) {
  if (isInView) {
    $(this).addClass('animate__animated animate__bounce');
  } else {
    $(this).removeClass('animate__animated animate__bounce');
  }
});
