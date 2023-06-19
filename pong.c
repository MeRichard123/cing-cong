#include <stdio.h>
#include "raylib.h"

#define HEIGHT 450
#define WIDTH 800
#define TITLE "Pong!"
#define PADDLEW 15
#define PADDLEH 100
#define BALLRADIUS 10

// Custom Structs

typedef struct {
  float dx;
  float dy;
} MotionVec;

// Function Prototypes

void resetBallPos(Vector2* posVec, MotionVec* motionVec);
void moveBall(Vector2* ball, MotionVec* mv);
void moveAi(Rectangle * AiPaddle, Vector2 * ball, MotionVec * mv);

// Implementation

void resetBallPos(Vector2* posVec, MotionVec* motionVec) {
  posVec->x = WIDTH/2;
  posVec->y = HEIGHT/2;
  // invert direction so each time is goes on a different side
  motionVec->dx = -motionVec->dx;
}

void moveBall(Vector2* ball, MotionVec* mv) {
  ball->x += mv->dx;
  ball->y += mv->dy;
}

void moveAi(Rectangle * AiPaddle, Vector2 * ball, MotionVec * mv) {
  int paddleCenter = AiPaddle -> y + (AiPaddle -> height / 2);
  int screenCenter = HEIGHT / 2;
  int ballSpeed = mv -> dy;
  // 1.5 seems to be a good middle ground. The movement is not too glitchy and 
  // it doesn't make the game impossible.
  // When it's around 3 it becomes impossible to get any points
  const float speedScalar = 1.5f;

  // ball moving left
  if (mv -> dx < 0) {
    if (AiPaddle -> y < HEIGHT) {
      if (paddleCenter < screenCenter) {
        AiPaddle -> y += ballSpeed + speedScalar;
      } else {
        AiPaddle -> y -= ballSpeed + speedScalar;
      }
    }
  }
  // ball moving right 
  else {
    if (mv -> dy > 0) {
      if (ball -> y > paddleCenter + (AiPaddle -> height / 2)) {
        AiPaddle -> y += ballSpeed + speedScalar;
      } else {
        AiPaddle -> y -= ballSpeed + speedScalar;
      }
    }
    if (mv -> dy < 0) {
      if (ball -> y < paddleCenter) {
        AiPaddle -> y -= ballSpeed + speedScalar;
      } else {
        AiPaddle -> y += ballSpeed + speedScalar;
      }
    }
  }
}


int main(void) {
  int score = 00;
  int AiScore = 00;

  char scoreText[20];
  char scoreTextAi[20];
  int padding = 10;
  const float accelectionConstant = 3.3; 
  
  Vector2 ballCenter = { (float)WIDTH/2, (float)HEIGHT/2 };
  
  MotionVec ballMotionVector = {
    .dx = -6.0f,
    .dy = 1.2f,
  };

  Rectangle LeftPaddle = {
    .x = (float)padding,
    .y = (float)HEIGHT/2 - PADDLEH/2,
    .width = PADDLEW,
    .height = PADDLEH,
  };

  Rectangle RightPaddle = {
    .x = (float)WIDTH - PADDLEW - padding,
    .y = (float)HEIGHT/2 - PADDLEH/2,
    .width = PADDLEW,
    .height = PADDLEH,
  };

  InitWindow(WIDTH, HEIGHT, TITLE);
  SetTargetFPS(60);
  
  // play music in the background
  InitAudioDevice();

  Music music = LoadMusicStream("./tetris.mp3");
  music.looping = true;

  PlayMusicStream(music);

  while(!WindowShouldClose()) {
    if (IsKeyDown(KEY_UP) && LeftPaddle.y-2.0f > -(LeftPaddle.height/2)) {
        LeftPaddle.y -= 2.0f;
    }
    if (IsKeyDown(KEY_DOWN) && LeftPaddle.y+2.0f+(LeftPaddle.height/2) < HEIGHT) {
      LeftPaddle.y += 2.0f;
    }

    UpdateMusicStream(music);

    moveBall(&ballCenter, &ballMotionVector);
    moveAi(&RightPaddle, &ballCenter, &ballMotionVector);
  
    // Check if ball has left the screen
    if (ballCenter.x > WIDTH + 100) {
      resetBallPos(&ballCenter, &ballMotionVector);
      score++;
    }
    if (ballCenter.x < -100) {
      resetBallPos(&ballCenter, &ballMotionVector);
      AiScore++;
    }

    // check if the ball has hit the top or bottom
    if (ballCenter.y > HEIGHT-BALLRADIUS || ballCenter.y < BALLRADIUS) {
      ballMotionVector.dy = -ballMotionVector.dy;
    }

    if (CheckCollisionCircleRec(ballCenter, BALLRADIUS, LeftPaddle)) {
      int paddleCenter = LeftPaddle.y + (LeftPaddle.height / 2);
      float diffScore = 0;
      
      if (ballCenter.y > paddleCenter) {
        int ballDistanceFromCenterOfPaddle = ballCenter.y - paddleCenter;
        diffScore = (1 + (ballDistanceFromCenterOfPaddle / (LeftPaddle.height/2)))*accelectionConstant;
      } 
      else if (ballCenter.y < paddleCenter) {
        int ballDistanceFromCenterOfPaddle = paddleCenter - ballCenter.y;
        diffScore = (1 + (ballDistanceFromCenterOfPaddle / (LeftPaddle.height/2)))*accelectionConstant;
      }
      else if (ballCenter.y == paddleCenter) {
        ballMotionVector.dy = 0;
      }

      ballMotionVector.dx += diffScore;
    }
    

    if (CheckCollisionCircleRec(ballCenter, BALLRADIUS, RightPaddle)) {
      // reduce velocity on collision 
      ballMotionVector.dx = ballMotionVector.dx/2;
      
      int paddleCenter = RightPaddle.y + (RightPaddle.height / 2);
      float diffScore = 0;

      if (ballCenter.y > paddleCenter) {
        int ballDistanceFromCenterOfPaddle = ballCenter.y - paddleCenter;
        diffScore = (1 + (ballDistanceFromCenterOfPaddle / (RightPaddle.height/2)))*accelectionConstant;
      }
      else if (ballCenter.y < paddleCenter) {
        int ballDistanceFromCenterOfPaddle = paddleCenter - ballCenter.y;
        diffScore = (1 + (ballDistanceFromCenterOfPaddle / (RightPaddle.height/2)))*accelectionConstant;
      }
      else if (ballCenter.y == paddleCenter) {
        ballMotionVector.dy = 0;
      }

      ballMotionVector.dx -= diffScore;
    }

    BeginDrawing();
      ClearBackground(BLACK);
      DrawFPS(15 + LeftPaddle.width ,35);

      // Put Score on the Screen
      sprintf(scoreTextAi, "Ai Score: %d", AiScore);
      DrawText(scoreTextAi, WIDTH-150, 10, 20, WHITE);
      sprintf(scoreText, "Score %d", score);
      DrawText(scoreText, 15 + LeftPaddle.width , 10, 20, WHITE);

      // Draw Paddles
      DrawRectangleRec(LeftPaddle, WHITE);
      DrawRectangleRec(RightPaddle, WHITE);
    
      // Draw Ball
      DrawCircleV(ballCenter, BALLRADIUS, WHITE);
      
      // Draw a Line down the middle.
      for (int i = 0; i < HEIGHT; i=i+40) {
        DrawLine(WIDTH/2, i, WIDTH/2, i + 20, WHITE);
        DrawLine(WIDTH/2, i + 20, WIDTH/2, i + 40, BLACK);
      }

    EndDrawing();
  }
  
  // Unload music stream buffers from RAM
  UnloadMusicStream(music);
  
  // Close audio device (music streaming is automatically stopped)
  CloseAudioDevice(); 
  
  CloseWindow();

  return 0;
}
