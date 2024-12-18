// components/countdown-manager.js
/**
 * @component countdown-manager
 * @description Manages turn-based countdown system between two cannons
 */
AFRAME.registerComponent('countdown-manager', {
    init: function() {
      this.isGreenTurn = true; // Start with green
      this.countdown = 5;
      this.isRunning = false;
      
      // Get references to both text elements
      this.greenText = document.querySelector('#greenTimer');
      this.blueText = document.querySelector('#blueTimer');
      
      // Start when either marker is found
      this.el.sceneEl.addEventListener('markerFound', () => {
        if (!this.isRunning) {
          this.startCountdown();
        }
      });
    },
  
    startCountdown: function() {
      this.isRunning = true;
      this.countdown = 5;
      
      const timer = setInterval(() => {
        // Update current player's text
        if (this.isGreenTurn) {
          this.greenText.setAttribute('value', this.countdown.toString());
          this.blueText.setAttribute('value', '');
        } else {
          this.blueText.setAttribute('value', this.countdown.toString());
          this.greenText.setAttribute('value', '');
        }
        
        this.countdown--;
        
        if (this.countdown < 0) {
          // Switch turns
          this.isGreenTurn = !this.isGreenTurn;
          this.countdown = 5;
          
          clearInterval(timer);
          setTimeout(() => {
            this.startCountdown();
          }, 500);
        }
      }, 1000);
    }
  });