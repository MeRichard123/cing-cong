#include <stdio.h>
#include "raylib.h"

#define HEIGHT 450
#define WIDTH 800
#define TITLE "Pong!"
#define PADDLEW 15
#define PADDLEH 100
#define BALLRADIUS 10

typedef struct {
  float dx;
  float dy;
} MotionVec;

void resetBallPos(Vector2* posVec, MotionVec* motionVec) {
  posVec->x = WIDTH/2;
  posVec->y = HEIGHT/2;
  motionVec->dx = -motionVec->dx;
}

void moveBall(Vector2* ball, MotionVec* mv) {
  ball->x += mv->dx;
  ball->y += mv->dy;
}

void moveAi(Rectangle* AiPaddle, Vector2* ball, MotionVec* mv) {
  int dy = 4.0f;
  int paddleCenter = AiPaddle->y + (AiPaddle->height / 2);
  int screenCenter = HEIGHT / 2;
  int ballSpeed = mv->dy;

  // ball moving left
  if (mv->dx < 0) {
    if ((AiPaddle->y)+AiPaddle->height < HEIGHT){
       if (paddleCenter < screenCenter) { 
          AiPaddle->y += ballSpeed + 2.5f;  
       } else {
          AiPaddle->y -= ballSpeed + 2.5f;
       }
    }
  }
  // ball moving right 
  else {
    if (mv->dy > 0) {
      if (ball->y > paddleCenter) {
        AiPaddle->y += ballSpeed + 2.5f;
      }else {
        AiPaddle->y -= ballSpeed + 2.5f;
      }
    }
	  if (mv->dy < 0) {
      if (ball->y < paddleCenter) { 
          AiPaddle->y -= ballSpeed + 2.5f;
      } else {
          AiPaddle->y += ballSpeed + 2.5f;
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
  
  Vector2 leftPaddlePosition = { 
         (float)padding, 
         (float)HEIGHT/2 - PADDLEH/2
  };

  Vector2 rightPaddlePosition = { 
         (float)WIDTH - PADDLEW - padding, 
         (float)HEIGHT/2 - PADDLEH/2
  };
    
  Vector2 ballCenter = { (float)WIDTH/2, (float)HEIGHT/2 };
  
  MotionVec ballMotionVector = {
    .dx = -6.0f,
    .dy = 1.2f,
  };

  Vector2 paddleSize = { PADDLEW, PADDLEH };

  Rectangle LeftPaddle = {
    .x = leftPaddlePosition.x,
    .y = leftPaddlePosition.y,
    .width = paddleSize.x,
    .height = paddleSize.y,
  };

  Rectangle RightPaddle = {
    .x = rightPaddlePosition.x,
    .y = rightPaddlePosition.y,
    .width = paddleSize.x,
    .height = paddleSize.y,
  };

  InitWindow(WIDTH, HEIGHT, TITLE);
  SetTargetFPS(60);
    
  while(!WindowShouldClose()) {
    if (IsKeyDown(KEY_UP) && LeftPaddle.y-2.0f > -(LeftPaddle.height/2)) {
        LeftPaddle.y -= 2.0f;
    }
    if (IsKeyDown(KEY_DOWN) && LeftPaddle.y+2.0f+(LeftPaddle.height/2) < HEIGHT) {
      LeftPaddle.y += 2.0f;
    }

    moveBall(&ballCenter, &ballMotionVector);
    moveAi(&RightPaddle, &ballCenter, &ballMotionVector);
  
    // Check if ball has left the screen
    if (ballCenter.x > WIDTH) {
      resetBallPos(&ballCenter, &ballMotionVector);
      score++;
    }
    if (ballCenter.x < 5) {
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
        diffScore = 1 + ((ballDistanceFromCenterOfPaddle / (LeftPaddle.height/2))*4.3);
      } 
      else if (ballCenter.y < paddleCenter) {
        int ballDistanceFromCenterOfPaddle = paddleCenter - ballCenter.y;
        diffScore = 1 + ((ballDistanceFromCenterOfPaddle / (LeftPaddle.height/2))*4.3);
      }

      ballMotionVector.dx += diffScore;
    }
    

    if (CheckCollisionCircleRec(ballCenter, BALLRADIUS, RightPaddle)) {
      // do something else (update motion vector)
      int paddleCenter = RightPaddle.y + (RightPaddle.height / 2);
      float diffScore = 0;

      if (ballCenter.y > paddleCenter) {
        int ballDistanceFromCenterOfPaddle = ballCenter.y - paddleCenter;
        diffScore = 1 + ((ballDistanceFromCenterOfPaddle / (RightPaddle.height/2))*4.3);
      }
      else if (ballCenter.y < paddleCenter) {
        int ballDistanceFromCenterOfPaddle = paddleCenter - ballCenter.y;
        diffScore = 1 + ((ballDistanceFromCenterOfPaddle / (RightPaddle.height/2))*4.3);
      }

      ballMotionVector.dx -= diffScore;
    }

    BeginDrawing();
      ClearBackground(BLACK);
      DrawFPS(10,35);

      // Put Score on the Screen
      sprintf(scoreTextAi, "Ai Score: %d", AiScore);
      DrawText(scoreTextAi, WIDTH-150, 10, 20, WHITE);
      sprintf(scoreText, "Score %d", score);
      DrawText(scoreText, 10, 10, 20, WHITE);

      // Draw Paddles
      DrawRectangleRec(LeftPaddle, WHITE);
      DrawRectangleRec(RightPaddle, WHITE);
    
      // Draw Ball
      DrawCircleV(ballCenter, BALLRADIUS, WHITE);


    EndDrawing();
  }
  CloseWindow();

  return 0;
}
