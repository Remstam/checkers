#include <cstdlib>
#include "glut.h"
#include "functions.h"
#include <cstdio>
#include <cmath>

void render()
{
	/*COLOR BUFFER BEGINS*/
	glClearColor(1.0,1.0,1.0,1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60,(GLfloat)width/height,0.1,100);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0,0,distZ);
	glRotatef(angleX,0,1,0);
	glRotatef(angleY,1,0,0);

	put_quads();

	glReadPixels(vx,height-vy,1,1,GL_RGB,GL_UNSIGNED_BYTE,&pixel);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glClearColor(0.9,0.6,0.6,1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	/*COLOR BUFFER ENDS*/

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60,(GLfloat)width/height,0.1,100);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0,0,distZ);
	glRotatef(angleX,0,1,0);
	glRotatef(angleY,1,0,0);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_TEXTURE_2D);

	/*Земля на фоне*/
	glPushMatrix();
	glLoadIdentity();
	glBindTexture(GL_TEXTURE_2D,1);
	glBegin(GL_QUAD_STRIP);
		glTexCoord2d(0,1); glVertex3f(10,-10,-10);
		glTexCoord2d(1,1); glVertex3f(10,10,-10);
		glTexCoord2d(0,0); glVertex3f(-10,-10,-10);
		glTexCoord2d(1,0); glVertex3f(-10,10,-10);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	/*Доска с текстутой*/
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,2);
	ppd(1,1,0.02);

	/*Блендинг с бликом на доске*/
	glEnable(GL_BLEND);
		glBlendFunc(GL_ONE,GL_ONE_MINUS_SRC_COLOR); /*велосипед с неправильными параметрами*/
		glBindTexture(GL_TEXTURE_2D,3);

		glPushMatrix();
			glTranslatef(-0.3*sin(angleX*PI/180),0.3*sin(angleY*PI/180),0);
			glBegin(GL_QUAD_STRIP);
				glTexCoord2d(0,1); glVertex3f(0.1*(2-cos(angleX*PI/180)),-0.1*(2-cos(angleY*PI/180)),0.0001);
				glTexCoord2d(1,1); glVertex3f(0.1*(2-cos(angleX*PI/180)),0.1*(2-cos(angleY*PI/180)),0.0001);
				glTexCoord2d(0,0); glVertex3f(-0.1*(2-cos(angleX*PI/180)),-0.1*(2-cos(angleY*PI/180)),0.0001);
				glTexCoord2d(1,0); glVertex3f(-0.1*(2-cos(angleX*PI/180)),0.1*(2-cos(angleY*PI/180)),0.0001);
			glEnd();
		glPopMatrix();

		glBindTexture(GL_TEXTURE_2D,0);
	glDisable(GL_BLEND);
	
	/*Самая сложнаяя часть*/
	if(place!=127) /*если ткнули в клетку*/
	{
		cellx=place%10; //считаем ее координаты
		celly=place/10;

		move=1;
		for(int i=0;i<8;i++)
			for(int j=0;j<8;j++)
				if(cellx==canx[i] && celly==cany[j]) move=2; //если ткнули в клетку, в которую можно
															 //сходить, то меняем статус действия на "ходить"

		if(move==1) //тычем в клетку
		{
			//if(button==-1)
			switch(map[cellx][celly])									//смотрим, что в клетке
			{
				case 0:													//если ничего нет
						draw_quad(red,0,0);							//то это клетка, на которой нет шашки
						for(int i=0;i<8;i++)						//плюс чистим доступные ходы, чтобы на следующем ходу ими не воспользовались
						{
							canx[i]=-1;
							cany[i]=-1;
						}
						break;
				case 1:													//желтая шашка
						draw_quad(yellow,0,0);							//подсветим ее
						globe=1;										//ожидается ход желтых
						for(int i=0;i<8;i++)
						{
							canx[i]=-1;
							cany[i]=-1;
						}
						if(!(cellx-1<0 || celly+1>7))					//если влево вверх не закончилась доска
						{
							switch(map[cellx-1][celly+1])				//смотрим, что там
							{
								case 0:									//если там пустая клетка
										draw_quad(yellow,-1,1);			//можно туда идти - подсветим
										canx[cellx-1]=cellx-1;
										cany[celly+1]=celly+1;
										break;
								case 2:									//если там синяя шашка
										if(!(cellx-2<0 || celly+2>7))	//если за синей шашкой не кончилась доска
											if(map[cellx-2][celly+2]==0) //если за синей шашкой пусто
											{
												draw_quad(yellow,-2,2); //то туда можно ходить
												canx[cellx-2]=cellx-2;
												cany[celly+2]=celly+2;
											}
										break;
							}
						}
						if(!(cellx+1>7 || celly+1>7))					//если вправо вверх не закончилась доска
						{
							switch(map[cellx+1][celly+1])
							{
								case 0:
										draw_quad(yellow,1,1);
										canx[cellx+1]=cellx+1;
										cany[celly+1]=celly+1;
										break;
								case 2:
										if(!(cellx+2>7 || celly+2>7))
											if(map[cellx+2][celly+2]==0)
											{
												draw_quad(yellow,2,2);
												canx[cellx+2]=cellx+2;
												cany[celly+2]=celly+2;
											}
										break;
							}
						}
						if(!(cellx-1<0 || celly-1<0))					//если влево вниз не закончилась доска
							if(map[cellx-1][celly-1]==2)
								if(!(cellx-2<0 || celly-2<0))
									if(map[cellx-2][celly-2]==0)
									{
										draw_quad(yellow,-2,-2);
										canx[cellx-2]=cellx-2;
										cany[celly-2]=celly-2;
									}
						if(!(cellx+1>7 || celly-1<0))
							if(map[cellx+1][celly-1]==2)
								if(!(cellx+2>7 || celly-2<0))
									if(map[cellx+2][celly-2]==0)
									{
										draw_quad(yellow,2,-2);
										canx[cellx+2]=cellx+2;
										cany[celly-2]=celly-2;
									}
						break;
				case 2:													//для синих все то же самое
						draw_quad(blue,0,0);
						globe=2;
						for(int i=0;i<8;i++)
						{
							canx[i]=-1;
							cany[i]=-1;
						}
						if(!(cellx+1>7 || celly-1<0))
						{
							switch(map[cellx+1][celly-1])
							{
								case 0:
										draw_quad(blue,1,-1);
										canx[cellx+1]=cellx+1;
										cany[celly-1]=celly-1;
										break;
								case 1:
										if(!(cellx+2>7 || celly-2<0))
											if(map[cellx+2][celly-2]==0)
											{
												draw_quad(blue,2,-2);
												canx[cellx+2]=cellx+2;
												cany[celly-2]=celly-2;
											}
										break;
							}
						}
						if(!(cellx-1<0 || celly-1<0))
						{
							switch(map[cellx-1][celly-1])
							{
								case 0:
										draw_quad(blue,-1,-1);
										canx[cellx-1]=cellx-1;
										cany[celly-1]=celly-1;
										break;
								case 1:
										if(!(cellx-2<0 || celly-2<0))
											if(map[cellx-2][celly-2]==0)
											{
												draw_quad(blue,-2,-2);
												canx[cellx-2]=cellx-2;
												cany[celly-2]=celly-2;
											}
										break;
							}
						}
						if(!(cellx+1>7 || celly+1>7))
							if(map[cellx+1][celly+1]==1)
								if(!(cellx+2>7 || celly+2>7))
									if(map[cellx+2][celly+2]==0)
									{
										draw_quad(blue,2,2);
										canx[cellx+2]=cellx+2;
										cany[celly+2]=celly+2;
									}
						if(!(cellx-1<0 || celly+1>7))
							if(map[cellx-1][celly+1]==1)
								if(!(cellx-2<0 || celly+2>7))
									if(map[cellx-2][celly+2]==0)
									{
										draw_quad(blue,-2,2);
										canx[cellx-2]=cellx-2;
										cany[celly+2]=celly+2;
									}
						break;
			}
		}
		else //в данный момент ходит шашка
		{
			if(stemp<1/* && move==2*/) //если шашка еще не переехала на новую клетку, то она еще едет
			{
				glPushMatrix();
				glTranslatef(((oldcellx+(cellx-oldcellx)*stemp)*2-7)*step,((oldcelly+(celly-oldcelly)*stemp)*2-7)*step,0);
				if(globe==1)
					glMaterialfv(GL_FRONT,GL_DIFFUSE,yellow);
				else
					glMaterialfv(GL_FRONT,GL_DIFFUSE,blue);
				if(abs(cellx-oldcellx)==2 && abs(celly-oldcelly)==2) //если производится сейчас взятие, то нужно приподнять шашку
					glTranslatef(0,0,(stemp<0.5)?(stemp/4):((1-stemp)/4));
				gluDisk(p,0,step,slice,slice);
				gluCylinder(p,step,step,0.02,slice,slice);
					glTranslatef(0,0,0.019);
				gluDisk(p,0,step,slice,slice);
				glutSolidTorus(0.01,step-0.01,slice,slice);
				glutSolidTorus(0.02,step-0.03,slice,slice);
				glutSolidCone(step-0.03,0.03,slice,slice);
				gluDisk(p,0,step-0.03,slice,slice);
				glMaterialfv(GL_FRONT,GL_DIFFUSE,white);
				glPopMatrix();

				if(abs(cellx-oldcellx)==2 && abs(celly-oldcelly)==2) //при взятии кушаемая шашка пропадает под доской
				{
					flag=1;
					tempx=(cellx>oldcellx)?cellx-1:cellx+1;
					tempy=(celly>oldcelly)?celly-1:celly+1;
					map[tempx][tempy]=0;

					glPushMatrix();
					glTranslatef((tempx*2-7)*step,(tempy*2-7)*step,-stemp/10);
					if(globe==2)
						glMaterialfv(GL_FRONT,GL_DIFFUSE,yellow);
					else
						glMaterialfv(GL_FRONT,GL_DIFFUSE,blue);
					gluDisk(p,0,step,slice,slice);
					gluCylinder(p,step,step,0.02,slice,slice);
					glTranslatef(0,0,0.019);
					gluDisk(p,0,step,slice,slice);
					glutSolidTorus(0.01,step-0.01,slice,slice);
					glutSolidTorus(0.02,step-0.03,slice,slice);
					glutSolidCone(step-0.03,0.03,slice,slice);
					gluDisk(p,0,step-0.03,slice,slice);
					glMaterialfv(GL_FRONT,GL_DIFFUSE,white);
					glPopMatrix();
				}

				stemp+=step/2;										//едем дальше
				map[oldcellx][oldcelly]=0;							//шашка уехала со старого места
			}
			else													//уже приехали
			{
				//if(move==2)
				//{
					map[cellx][celly]=(globe==1)?1:2;				//на новом месте ставим шашку нужного цвета
					//map[oldcellx][oldcelly]=0;
				//}
				for(int i=0;i<8;i++)								//чистим доступыне ходы
				{
					canx[i]=-1;
					cany[i]=-1;
				}
				if(flag)(globe==1)?yellow_score++:blue_score++;		//очко команде, съевшей шашку
				flag=0;
			}
		}
	}

	/*Ставим шашки на доску*/
	put_checkers();

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	/*Отобразим счет*/
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0,width, 0,height, -1,1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glColor3f(1,1,0);

	/*Собственно, счет*/
	if(yellow_score>blue_score)
		sprintf(sz, "Yellows lead %d to %d", yellow_score, blue_score);
	else if(yellow_score<blue_score)
		sprintf(sz, "Blues lead %d to %d", blue_score, yellow_score);
	else sprintf(sz, "Teams are tied at %d-%d", yellow_score, blue_score);
	draw_string(GLUT_BITMAP_TIMES_ROMAN_24, sz, 10, height-30);

	/*Старый отладочный вывод*/
	/*sprintf(sz, "coord: (%d, %d, %.3f, %i, %i, %i, %i); ", vx, height-vy, stemp, cellx, celly, oldcellx, oldcelly);
	draw_string(GLUT_BITMAP_TIMES_ROMAN_24, sz, 10, 50);
	sprintf(sz, "border: (%d %.3f %.3f)", move, angleX, angleY);
	draw_string(GLUT_BITMAP_TIMES_ROMAN_24, sz, 10, 30);*/
	
	glFlush();

	glutSwapBuffers();
}

int main(int argc, char *argv)
{
	glutInit(&argc, &argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(800,600);
	glutCreateWindow("Checkers");
	texture_from_file("back.bmp",1);
	texture_from_file("board2.bmp",2);
	texture_from_file("bl3.bmp",3);
	//начало регистрации обработчиков сообщений
	glutDisplayFunc(render);
	glutReshapeFunc(on_size);
	glutIdleFunc(render);
	glutMouseFunc(mouse_button);
	glutMotionFunc(mouse_move);
	//конец регистрации
	glutMainLoop();
	return 0;
}