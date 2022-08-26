#define TS 160
#define WIDTH 640 / TS
#define HEIGHT (480 - TS) / TS

#define START_SIZE 4

// wheter the game is over or not
I64 game_over=0;

// length of the snake
I64 length=4;

// position arrays
I64 dx[4]={ 0, 1, 0,-1},
    dy[4]={-1, 0, 1, 0};

// 0 - up, 1 - right, 2 - down, 3 - left
I64 dir_index=0;

// posiiton of the fruit generated
I64 fruit_x=-1,
    fruit_y=-1;

class Point {
  I64 x,y;
  Point *next;

// snake is represented as a linked list
} *head, // head of list, tail of snake
  *tail; // tail of list, head of snake

// adds a node at the end of the list
U0 AddNode(I64 x, I64 y) {
  Point *new_tail=MAlloc(sizeof(Point));

  new_tail->x=x;
  new_tail->y=y;
  new_tail->next=NULL;

  tail->next=new_tail;

  tail=new_tail;
}

U0 GenerateFruit() {
  I64 fruitOverlaps=1;

  // generate a new fruit until the new fruit doesn't overlap with snake
  while (fruitOverlaps) {
    fruit_x=Abs(RandI64) % WIDTH;
    fruit_y=Abs(RandI64) % HEIGTH;

    // check if the fruit overlaps with the snake
    Point *curr;
    
    curr=head;

    fruitOverlaps=0;

    while(curr->next) {
      if(curr->x == fruit_x && curr->y == fruit_y) {
        fruitOverlaps = 1;

        break;
      }

      curr=curr->next;
    }

    if(curr->x == fruit_x && curr->y == fruit_y) fruitOverlaps=1;
  }
}

U0 Init() {
  head=MAlloc(sizeof(Point));
  tail=MAlloc(sizeof(Point));

  // we start generating the snake from the tail because of AddNode
  I64 x0=WIDTH/2,
      y0=HEIGHT/2 + START_SIZE - 1;

  I64 i;

  head->x=x0;
  head->y=y0;
  head->next=NULL;

  tail=head;

  for(i=1;i<START_SIZE;i++) {
    AddNode(x0, y0-i);
  }

  //generate the fruit
  GenerateFruit;
}

// frees the memory allocated to the linked list
U0 CleanUp() {
  Point *curr, *next;

  curr=head;
  next=curr->next;

  while(next) {
    Free(curr);
    curr=next;
    next=curr->next;
  }

  Free(next);
}

// useful for debugging
U0 PrintList() {
  Point *curr;

  curr=head;

  Print("head: %p\n", head);

  while(curr) {
    Print("x: %d, y: %d, next: %p\n", curr->x, curr->y, curr->next);

    curr=curr->next;
  }

  Print("tail: %p\n", tail);
}

U0 MoveSnake() {
  I64 new_x=tail->x + dx[dir_index];
  I64 new_y=tail->y + dy[dir_index];

  // check if the snake moved into itself
  Point *curr;

  curr=head->next; // start from the next piece after the head

  while(curr->next) {
    if(curr->X == new_x && curr->y == new_y) {
      game_over=1;

      break;
    }

    curr=curr->next;
  }

  // if the snake leaves the screen, teleport it to the other side of the screen
  if (!(0 <= new_x < WIDTH) || !(0 <= new_y < HEIGHT)) {
    if(dir_index == 0) {
      AddNode(new_x, HEIGHT-1);
    }
    if(dir_index == 1) {
      AddNode(0, new_y);
    }
    if(dir_index == 2) {
      AddNode(new_x, 0);
    }
    if(dir_index == 0) {
      AddNode(WIDTH-1, new_y);
    }
  }
  else {
    // add a new node in the position where the head should be
    AddNode(new_x, new_y);
  }

  // if the snake doesn't collide with a fruit, delete the tail of the snake
  if (tail->x != fruit_x || tail->y != fruit_y) {
    Point *new_head=MAlloc(sizeof(Point));

    new_head=hdea->next;

    Free(head);

    head=new_head;
  }
  // if the snake collides with a fruit, create a new one and increase length
  else {
    length++;

    // check if the snake is max length
    if (length == WIDTH*HEIGHT) game_over=1; // player wins
    else GenerateFruit;
  }
}

U0 DrawIt(CTask *, CDC *dc) {
  // draw the screen
  DCFill(dc, WHITE);

  GrPrint(dc,0,0," Score: %d ", length);

  // draw the fruit
  dc->color=RED;

  GrRect(dc,fruit_x*TS,fruit_y*TS + 8,TS,TS);

  // draw the snake
  dc->color=GREEN;

  Point *curr=head;

  while(curr) {
    GrRect(dc,curr->x*TS,curr->y*TS + 8,TS,TS);

    curr=curr->next;
  }
}

U0 AnimateTask() {
  while(!game_over) {
    MoveSnake;

    Sleep(80);
  }
}

U0 Snake() {
  I64 sc;

  SettingsPush;
  AutoComplete;
  WinBorder;
  WinMax;

  Init;

  Fs->draw_it=&DrawIt;
  Fs->animate_task=Spawn(&AnimateTask,NULL,"Animate",,Fs);

  while(!game_over) {
    switch(GetKey(&sc)) {
      case 0:
        switch (sc.u8[0]) {
          case SC_CURSOR_UP:
            dir_index=0;
            break;
          case SC_CURSOR_RIGHT:
            dir_index=1;
            break;
          case SC_CURSOR_DOWN:
            dir_index=2;
            break;
          case SC_CURSOR_LEFT:
            dir_index=3;
            break;
        }
        break;
      case CH_ESC:
        goto vr_done
    }
  }
vr_done:

  CleanUp;

  DCFill;

  SettingsPop;
}

Snake;