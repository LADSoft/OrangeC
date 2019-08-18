/*

1.61803398874989484820458683436563811772030917980576286213544862270526046281890
2449707207204189391137484754088075386891752126633862223536931793180060766726354
4333890865959395829056383226613199282902678806752087668925017116962070322210432
1626954862629631361443814975870122034080588795445474924618569536486444924104432
0771344947049565846788509874339442212544877066478091588460749988712400765217057
5179788341662562494075890697040002812104276217711177780531531714101170466659914
6697987317613560067087480710131795236894275219484353056783002287856997829778347
8458782289110976250030269615617002504643382437764861028383126833037242926752631
1653392473167111211588186385133162038400522216579128667529465490681131715993432
3597349498509040947621322298101726107059611645629909816290555208524790352406020
1727997471753427775927786256194320827505131218156285512224809394712341451702237
3580577278616008688382952304592647878017889921990270776903895321968198615143780
3149974110692608867429622675756052317277752035361393621076738937645560606059216
5894667595519004005559089502295309423124823552122124154440064703405657347976639
7239494994658457887303962309037503399385621024236902513868041457799569812244574
7178034173126453220416397232134044449487302315417676893752103068737880344170093
9544096279558986787232095124268935573097045095956844017555198819218020640529055
1893494759260073485228210108819464454422231889131929468962200230144377026992300
7803085261180754519288770502109684249362713592518760777884665836150238913493333
1223105339232136243192637289106705033992822652635562090297

*/

#include <stdio.h>
#include <stdlib.h>

int main()
{
  int x[32768];
	int xc;
	for(xc = 0; xc < 32768; xc++) x[xc] = 0;
	xc=0;
	x[xc]+=6;
	while(x[xc] != 0)
	{
		xc++;
		x[xc]+=8;
		xc--;
		x[xc]--;
	}
	xc++;
	x[xc]++;
	putchar(x[xc]);
	x[xc]-=3;
	putchar(x[xc]);
	xc+=3;
	x[xc]++;
	xc+=2;
	x[xc]++;
	xc++;
	x[xc]+=2;
	xc++;
	x[xc]++;
	xc++;
	x[xc]++;
	xc++;
	x[xc]++;
	xc+=2;
	x[xc]++;
	xc++;
	x[xc]+=3;
	xc++;
	x[xc]++;
	xc+=2;
	x[xc]++;
	xc++;
	x[xc]++;
	xc++;
	x[xc]++;
	xc+=2;
	x[xc]++;
	xc+=4;
	x[xc]++;
	xc+=4;
	x[xc]++;
	xc++;
	x[xc]+=2;
	xc+=4;
	x[xc]++;
	xc++;
	x[xc]++;
	xc--;
	while(x[xc] != 0)
	{
		while(x[xc] != 0)
		{
			xc+=3;
		}
		xc-=3;
		while(x[xc] != 0)
		{
			xc+=8;
			xc++;
			x[xc]++;
			xc-=8;
			while(x[xc] != 0)
			{
				xc+=9;
				x[xc]++;
				xc-=9;
				x[xc]--;
			}
			xc--;
			x[xc]--;
			xc-=3;
		}
		xc+=6;
		x[xc]++;
		xc+=3;
		x[xc]++;
		while(x[xc] != 0)
		{
			xc-=3;
		}
		xc-=5;
		while(x[xc] != 0)
		{
			xc+=3;
			x[xc]++;
			xc-=2;
			while(x[xc] != 0)
			{
				xc+=3;
				x[xc]++;
				xc-=3;
				x[xc]--;
			}
			xc--;
			x[xc]--;
			xc-=3;
		}
		xc+=3;
		x[xc]++;
		xc-=8;
		while(x[xc] != 0)
		{
			xc-=2;
		}
		xc+=2;
		xc+=2;
		while(x[xc] != 0)
		{
			x[xc]--;
			xc++;
			while(x[xc] != 0)
			{
				xc-=3;
				x[xc]++;
				xc+=4;
				while(x[xc] != 0)
				{
					xc+=2;
				}
				xc++;
				x[xc]++;
				xc-=3;
				while(x[xc] != 0)
				{
					xc-=2;
				}
				xc++;
				x[xc]--;
			}
			xc-=3;
			x[xc]--;
			while(x[xc] != 0)
			{
				xc+=3;
				x[xc]++;
				xc-=3;
				x[xc]--;
			}
			x[xc]++;
			xc+=4;
			while(x[xc] != 0)
			{
				xc+=2;
			}
			x[xc]++;
			xc++;
			while(x[xc] != 0)
			{
				x[xc]--;
				while(x[xc] != 0)
				{
					x[xc]--;
					while(x[xc] != 0)
					{
						x[xc]--;
						xc--;
						x[xc]--;
						xc+=6;
						while(x[xc] != 0)
						{
							while(x[xc] != 0)
							{
								xc+=3;
							}
							xc+=2;
						}
						xc-=5;
						while(x[xc] != 0)
						{
							xc++;
							x[xc]++;
							xc--;
							x[xc]--;
							xc-=3;
						}
						xc-=2;
						while(x[xc] != 0)
						{
							xc-=3;
						}
						xc++;
						xc+=2;
						while(x[xc] != 0)
						{
							x[xc]--;
							xc+=3;
							while(x[xc] != 0)
							{
								xc+=3;
							}
							xc+=2;
							while(x[xc] != 0)
							{
								xc+=3;
							}
							x[xc]++;
							while(x[xc] != 0)
							{
								xc-=3;
							}
							xc-=2;
							while(x[xc] != 0)
							{
								xc-=3;
							}
							x[xc]++;
							xc+=3;
						}
						xc+=3;
						while(x[xc] != 0)
						{
							xc--;
							while(x[xc] != 0)
							{
								x[xc]--;
							}
							x[xc]++;
							xc++;
							x[xc]--;
							xc+=2;
							xc++;
						}
						xc--;
						while(x[xc] != 0)
						{
							xc+=3;
						}
						xc-=3;
						while(x[xc] != 0)
						{
							xc++;
							x[xc]+=9;
							xc-=4;
						}
						xc-=2;
						while(x[xc] != 0)
						{
							xc-=3;
						}
						xc+=3;
						while(x[xc] != 0)
						{
							x[xc]--;
							xc++;
							while(x[xc] != 0)
							{
								xc-=2;
								x[xc]++;
								xc++;
								x[xc]++;
								xc++;
								x[xc]--;
							}
							xc--;
							while(x[xc] != 0)
							{
								xc++;
								x[xc]++;
								xc--;
								x[xc]--;
							}
							xc++;
							while(x[xc] != 0)
							{
								xc+=2;
								while(x[xc] != 0)
								{
									xc+=3;
								}
								xc+=4;
								while(x[xc] != 0)
								{
									xc+=3;
								}
								xc--;
								x[xc]--;
								xc-=2;
								while(x[xc] != 0)
								{
									xc-=3;
								}
								xc-=4;
								while(x[xc] != 0)
								{
									xc-=3;
								}
								xc++;
								x[xc]--;
							}
							xc+=2;
							while(x[xc] != 0)
							{
								xc+=2;
								xc++;
							}
							xc+=4;
							while(x[xc] != 0)
							{
								xc+=3;
							}
							x[xc]++;
							while(x[xc] != 0)
							{
								xc-=3;
							}
							xc-=4;
							while(x[xc] != 0)
							{
								xc-=3;
							}
							xc--;
							while(x[xc] != 0)
							{
								xc+=2;
								x[xc]++;
								xc-=2;
								x[xc]--;
							}
							xc++;
							x[xc]++;
							xc+=3;
						}
						xc+=4;
						while(x[xc] != 0)
						{
							x[xc]--;
							xc+=3;
						}
						xc-=2;
						while(x[xc] != 0)
						{
							xc+=3;
						}
						xc-=3;
						while(x[xc] != 0)
						{
							xc++;
							while(x[xc] != 0)
							{
								x[xc]--;
								while(x[xc] != 0)
								{
									x[xc]--;
									while(x[xc] != 0)
									{
										x[xc]--;
										while(x[xc] != 0)
										{
											x[xc]--;
											while(x[xc] != 0)
											{
												x[xc]--;
												while(x[xc] != 0)
												{
													x[xc]--;
													while(x[xc] != 0)
													{
														x[xc]--;
														while(x[xc] != 0)
														{
															x[xc]--;
															while(x[xc] != 0)
															{
																x[xc]--;
																xc--;
																x[xc]--;
																xc++;
																while(x[xc] != 0)
																{
																	xc--;
																	x[xc]++;
																	xc++;
																	while(x[xc] != 0)
																	{
																		x[xc]--;
																	}
																	xc++;
																	x[xc]++;
																	xc--;
																}
															}
														}
													}
												}
											}
										}
									}
								}
							}
							xc--;
							while(x[xc] != 0)
							{
								while(x[xc] != 0)
								{
									xc+=2;
									while(x[xc] != 0)
									{
										xc-=3;
										x[xc]++;
										xc+=3;
										x[xc]--;
									}
									xc--;
									while(x[xc] != 0)
									{
										x[xc]--;
									}
									xc--;
									x[xc]--;
									xc-=3;
								}
								xc+=3;
							}
							xc-=3;
						}
						xc++;
						x[xc]++;
						xc++;
						while(x[xc] != 0)
						{
							x[xc]--;
							xc--;
							x[xc]--;
							xc-=3;
							while(x[xc] != 0)
							{
								while(x[xc] != 0)
								{
									xc--;
									xc-=2;
								}
								xc-=2;
							}
							xc-=3;
							while(x[xc] != 0)
							{
								xc-=2;
							}
							x[xc]++;
							xc+=2;
							x[xc]--;
							xc+=2;
							while(x[xc] != 0)
							{
								xc+=2;
							}
							xc+=7;
							x[xc]--;
							xc--;
							while(x[xc] != 0)
							{
								while(x[xc] != 0)
								{
									xc+=3;
								}
								xc+=2;
							}
							xc--;
						}
						xc--;
						while(x[xc] != 0)
						{
							x[xc]--;
							xc-=3;
							while(x[xc] != 0)
							{
								while(x[xc] != 0)
								{
									xc-=3;
								}
								xc-=2;
							}
							xc-=3;
							while(x[xc] != 0)
							{
								xc-=2;
							}
							x[xc]++;
							while(x[xc] != 0)
							{
								xc-=2;
							}
							xc+=2;
							x[xc]--;
							xc+=2;
							while(x[xc] != 0)
							{
								xc+=2;
							}
							xc+=6;
							while(x[xc] != 0)
							{
								while(x[xc] != 0)
								{
									xc+=3;
								}
								xc+=2;
							}
							xc-=2;
						}
						xc-=3;
						while(x[xc] != 0)
						{
							while(x[xc] != 0)
							{
								xc-=3;
							}
							xc-=2;
						}
					}
					xc--;
					while(x[xc] != 0)
					{
						x[xc]--;
						xc+=7;
						x[xc]++;
						while(x[xc] != 0)
						{
							xc-=2;
							x[xc]++;
							xc++;
							x[xc]++;
							xc++;
							x[xc]--;
						}
						xc--;
						x[xc]--;
						while(x[xc] != 0)
						{
							xc++;
							x[xc]++;
							xc--;
							x[xc]--;
						}
						x[xc]++;
						xc--;
						while(x[xc] != 0)
						{
							x[xc]--;
							while(x[xc] != 0)
							{
								x[xc]--;
								while(x[xc] != 0)
								{
									x[xc]--;
									while(x[xc] != 0)
									{
										x[xc]--;
										while(x[xc] != 0)
										{
											x[xc]--;
											while(x[xc] != 0)
											{
												x[xc]--;
												while(x[xc] != 0)
												{
													x[xc]--;
													while(x[xc] != 0)
													{
														x[xc]--;
														while(x[xc] != 0)
														{
															x[xc]--;
															while(x[xc] != 0)
															{
																x[xc]--;
																xc+=2;
																x[xc]--;
																xc-=9;
																while(x[xc] != 0)
																{
																	xc-=2;
																}
																x[xc]++;
																xc+=8;
																x[xc]--;
																xc+=2;
																while(x[xc] != 0)
																{
																	xc+=2;
																}
																xc+=5;
															}
														}
													}
												}
											}
										}
									}
								}
							}
						}
						xc-=5;
					}
					xc++;
				}
				xc--;
				while(x[xc] != 0)
				{
					x[xc]--;
					xc+=7;
					while(x[xc] != 0)
					{
						xc-=3;
						x[xc]++;
						xc++;
						x[xc]++;
						xc+=2;
						x[xc]--;
					}
					xc-=2;
					while(x[xc] != 0)
					{
						xc+=2;
						x[xc]++;
						xc-=2;
						x[xc]--;
					}
					x[xc]++;
					xc--;
					while(x[xc] != 0)
					{
						xc++;
						x[xc]--;
						xc--;
						while(x[xc] != 0)
						{
							xc+=2;
							while(x[xc] != 0)
							{
								xc+=3;
							}
							xc+=2;
							while(x[xc] != 0)
							{
								xc+=3;
							}
							xc+=2;
							while(x[xc] != 0)
							{
								xc+=3;
							}
							xc-=3;
							while(x[xc] != 0)
							{
								x[xc]--;
								xc-=3;
							}
							xc-=2;
							while(x[xc] != 0)
							{
								xc-=3;
							}
							xc-=2;
							while(x[xc] != 0)
							{
								xc-=3;
							}
							xc+=3;
							while(x[xc] != 0)
							{
								x[xc]--;
								xc++;
								while(x[xc] != 0)
								{
									xc-=2;
									x[xc]++;
									xc++;
									x[xc]++;
									xc++;
									x[xc]--;
								}
								xc--;
								while(x[xc] != 0)
								{
									xc++;
									x[xc]++;
									xc--;
									x[xc]--;
								}
								xc--;
								while(x[xc] != 0)
								{
									xc+=4;
									while(x[xc] != 0)
									{
										xc+=3;
									}
									xc+=2;
									while(x[xc] != 0)
									{
										xc+=3;
									}
									xc+=2;
									while(x[xc] != 0)
									{
										xc+=3;
									}
									xc++;
									x[xc]+=2;
									xc-=3;
									xc--;
									while(x[xc] != 0)
									{
										xc-=3;
									}
									xc-=2;
									while(x[xc] != 0)
									{
										xc-=3;
									}
									xc-=2;
									while(x[xc] != 0)
									{
										xc-=3;
									}
									xc--;
									x[xc]--;
								}
								xc+=4;
								while(x[xc] != 0)
								{
									xc+=3;
								}
								xc+=2;
								while(x[xc] != 0)
								{
									xc+=3;
								}
								xc+=2;
								while(x[xc] != 0)
								{
									xc+=3;
								}
								x[xc]++;
								while(x[xc] != 0)
								{
									xc-=2;
									xc--;
								}
								xc-=2;
								while(x[xc] != 0)
								{
									xc-=3;
								}
								xc-=2;
								while(x[xc] != 0)
								{
									xc-=3;
								}
								x[xc]++;
								xc+=3;
							}
							xc-=3;
							while(x[xc] != 0)
							{
								xc-=3;
							}
							xc++;
							x[xc]--;
						}
						xc+=2;
						while(x[xc] != 0)
						{
							xc+=3;
						}
						xc+=2;
						while(x[xc] != 0)
						{
							xc+=3;
						}
						xc+=2;
						while(x[xc] != 0)
						{
							x[xc]--;
							xc++;
							while(x[xc] != 0)
							{
								xc--;
								x[xc]++;
								xc+=2;
								x[xc]++;
								xc--;
								x[xc]--;
							}
							xc--;
							while(x[xc] != 0)
							{
								while(x[xc] != 0)
								{
									xc--;
									x[xc]++;
									xc+=2;
									x[xc]++;
									xc--;
									x[xc]--;
								}
								xc--;
								while(x[xc] != 0)
								{
									xc++;
									x[xc]++;
									xc--;
									x[xc]--;
								}
								xc++;
								x[xc]--;
								xc++;
								x[xc]--;
								while(x[xc] != 0)
								{
									xc--;
									x[xc]--;
									xc++;
									x[xc]--;
									while(x[xc] != 0)
									{
										xc--;
										x[xc]--;
										xc++;
										x[xc]--;
										while(x[xc] != 0)
										{
											xc--;
											x[xc]--;
											xc++;
											x[xc]--;
											while(x[xc] != 0)
											{
												xc--;
												x[xc]--;
												xc++;
												x[xc]--;
												while(x[xc] != 0)
												{
													xc--;
													x[xc]--;
													xc++;
													x[xc]--;
													while(x[xc] != 0)
													{
														xc--;
														x[xc]--;
														xc++;
														x[xc]--;
														while(x[xc] != 0)
														{
															xc--;
															x[xc]--;
															xc++;
															x[xc]--;
															while(x[xc] != 0)
															{
																xc--;
																x[xc]--;
																xc++;
																x[xc]--;
																while(x[xc] != 0)
																{
																	xc--;
																	x[xc]--;
																	xc++;
																	while(x[xc] != 0)
																	{
																		x[xc]--;
																	}
																	xc++;
																	x[xc]-=10;
																	xc++;
																	while(x[xc] != 0)
																	{
																		x[xc]--;
																	}
																	x[xc]++;
																	xc++;
																	x[xc]++;
																	xc-=3;
																}
															}
														}
													}
												}
											}
										}
									}
								}
								xc--;
							}
							x[xc]++;
							xc+=2;
							while(x[xc] != 0)
							{
								xc--;
								x[xc]++;
								xc++;
								x[xc]--;
							}
							xc++;
						}
						xc-=3;
						while(x[xc] != 0)
						{
							while(x[xc] != 0)
							{
								xc-=3;
							}
							xc-=2;
						}
						xc+=6;
						while(x[xc] != 0)
						{
							xc-=2;
							x[xc]++;
							xc+=3;
							x[xc]++;
							xc--;
							x[xc]--;
						}
						xc++;
						while(x[xc] != 0)
						{
							xc--;
							x[xc]++;
							xc++;
							x[xc]--;
						}
						xc-=3;
						while(x[xc] != 0)
						{
							xc++;
							while(x[xc] != 0)
							{
								while(x[xc] != 0)
								{
									xc+=3;
								}
								xc+=2;
							}
							xc--;
							x[xc]++;
							xc-=4;
							while(x[xc] != 0)
							{
								while(x[xc] != 0)
								{
									xc-=3;
								}
								xc-=2;
							}
							xc+=4;
							x[xc]--;
						}
						xc++;
						while(x[xc] != 0)
						{
							while(x[xc] != 0)
							{
								xc+=3;
							}
							xc+=2;
						}
						xc++;
						x[xc]++;
						xc-=2;
						x[xc]--;
						while(x[xc] != 0)
						{
							xc++;
							x[xc]++;
							xc--;
							x[xc]--;
						}
						xc++;
						while(x[xc] != 0)
						{
							xc++;
							x[xc]+=3;
							xc--;
							x[xc]--;
							while(x[xc] != 0)
							{
								xc++;
								x[xc]+=5;
								xc--;
								x[xc]--;
								while(x[xc] != 0)
								{
									xc++;
									x[xc]-=3;
									xc+=2;
									x[xc]++;
									xc++;
									x[xc]++;
									xc-=4;
									x[xc]--;
									while(x[xc] != 0)
									{
										xc++;
										x[xc]--;
										xc++;
										xc+=2;
										x[xc]++;
										xc-=4;
										x[xc]--;
										while(x[xc] != 0)
										{
											xc++;
											x[xc]++;
											xc+=3;
											x[xc]++;
											xc-=4;
											x[xc]--;
											while(x[xc] != 0)
											{
												xc++;
												x[xc]+=3;
												xc+=3;
												x[xc]++;
												xc-=4;
												x[xc]--;
												while(x[xc] != 0)
												{
													xc++;
													x[xc]-=5;
													xc+=3;
													x[xc]+=2;
													xc-=4;
													x[xc]--;
													while(x[xc] != 0)
													{
														xc++;
														x[xc]-=3;
														xc+=3;
														x[xc]+=2;
														xc-=4;
														x[xc]--;
													}
												}
											}
										}
									}
								}
							}
						}
						x[xc]++;
						while(x[xc] != 0)
						{
							while(x[xc] != 0)
							{
								xc-=3;
							}
							xc-=2;
						}
						xc+=3;
					}
					xc++;
					while(x[xc] != 0)
					{
						x[xc]--;
						xc++;
						while(x[xc] != 0)
						{
							while(x[xc] != 0)
							{
								xc+=3;
							}
							xc+=2;
						}
						x[xc]++;
						xc+=5;
						x[xc]++;
						while(x[xc] != 0)
						{
							while(x[xc] != 0)
							{
								xc-=3;
							}
							xc-=2;
						}
						xc+=4;
					}
					xc-=5;
				}
				xc++;
			}
			xc--;
			while(x[xc] != 0)
			{
				x[xc]--;
				xc+=6;
				while(x[xc] != 0)
				{
					while(x[xc] != 0)
					{
						xc+=3;
					}
					xc+=2;
				}
				xc-=2;
				xc-=3;
				while(x[xc] != 0)
				{
					xc-=3;
				}
				xc+=3;
				while(x[xc] != 0)
				{
					x[xc]--;
					xc-=3;
					while(x[xc] != 0)
					{
						xc-=3;
					}
					xc-=2;
					while(x[xc] != 0)
					{
						xc-=3;
					}
					xc+=2;
					while(x[xc] != 0)
					{
						xc+=3;
					}
					x[xc]++;
					xc++;
					while(x[xc] != 0)
					{
						xc+=3;
					}
					xc+=2;
					while(x[xc] != 0)
					{
						xc+=3;
					}
					xc++;
					while(x[xc] != 0)
					{
						xc-=4;
						while(x[xc] != 0)
						{
							xc-=3;
						}
						xc-=2;
						while(x[xc] != 0)
						{
							xc-=3;
						}
						xc+=2;
						while(x[xc] != 0)
						{
							xc+=3;
						}
						xc-=3;
						x[xc]++;
						xc++;
						while(x[xc] != 0)
						{
							xc+=3;
						}
						xc+=2;
						while(x[xc] != 0)
						{
							xc+=3;
						}
						xc++;
						x[xc]--;
					}
					xc--;
					x[xc]++;
					xc+=3;
				}
				xc-=3;
				while(x[xc] != 0)
				{
					x[xc]--;
					xc-=3;
				}
				xc-=2;
				while(x[xc] != 0)
				{
					xc-=3;
				}
				xc+=2;
				while(x[xc] != 0)
				{
					x[xc]--;
					xc+=3;
				}
				xc-=2;
				while(x[xc] != 0)
				{
					xc-=3;
				}
				xc+=3;
				while(x[xc] != 0)
				{
					x[xc]+=8;
					xc--;
					while(x[xc] != 0)
					{
						xc++;
						x[xc]--;
						xc--;
						x[xc]--;
					}
					xc++;
					while(x[xc] != 0)
					{
						xc++;
						x[xc]++;
						xc--;
						x[xc]--;
					}
					x[xc]++;
					xc+=3;
				}
				xc-=3;
				while(x[xc] != 0)
				{
					xc-=3;
				}
				xc+=4;
				x[xc]++;
				xc--;
				while(x[xc] != 0)
				{
					x[xc]--;
					xc++;
					while(x[xc] != 0)
					{
						xc-=2;
						x[xc]++;
						xc++;
						x[xc]++;
						xc++;
						x[xc]--;
					}
					xc--;
					while(x[xc] != 0)
					{
						x[xc]--;
						while(x[xc] != 0)
						{
							x[xc]--;
							while(x[xc] != 0)
							{
								x[xc]--;
								while(x[xc] != 0)
								{
									x[xc]--;
									while(x[xc] != 0)
									{
										x[xc]--;
										while(x[xc] != 0)
										{
											x[xc]--;
											while(x[xc] != 0)
											{
												x[xc]--;
												while(x[xc] != 0)
												{
													x[xc]--;
													while(x[xc] != 0)
													{
														x[xc]--;
														while(x[xc] != 0)
														{
															xc--;
															x[xc]-=10;
															xc+=4;
															while(x[xc] != 0)
															{
																x[xc]--;
															}
															x[xc]++;
															xc++;
															x[xc]++;
															xc-=4;
															while(x[xc] != 0)
															{
																x[xc]--;
															}
														}
													}
												}
											}
										}
									}
								}
							}
						}
					}
					xc--;
					while(x[xc] != 0)
					{
						xc+=2;
						x[xc]++;
						xc-=2;
						x[xc]--;
					}
					xc++;
					x[xc]++;
					xc+=3;
				}
				xc-=2;
				while(x[xc] != 0)
				{
					x[xc]--;
					xc--;
				}
				xc-=2;
				while(x[xc] != 0)
				{
					xc-=3;
				}
				xc+=3;
				x[xc]--;
				xc++;
				while(x[xc] != 0)
				{
					xc-=2;
					x[xc]++;
					xc+=2;
					x[xc]--;
				}
				xc+=2;
				while(x[xc] != 0)
				{
					xc+=3;
				}
				xc-=3;
				while(x[xc] != 0)
				{
					xc+=2;
					x[xc]++;
					xc--;
					while(x[xc] != 0)
					{
						xc++;
						x[xc]--;
						xc-=2;
						while(x[xc] != 0)
						{
							xc-=3;
						}
						xc++;
					}
					xc++;
					while(x[xc] != 0)
					{
						x[xc]--;
						xc-=2;
						x[xc]--;
						xc--;
					}
					xc-=2;
				}
				x[xc]++;
				xc--;
				while(x[xc] != 0)
				{
					xc+=2;
					x[xc]++;
					xc-=2;
					x[xc]--;
				}
				xc++;
				while(x[xc] != 0)
				{
					while(x[xc] != 0)
					{
						xc-=3;
					}
					xc-=2;
				}
				xc--;
			}
			xc-=2;
			while(x[xc] != 0)
			{
				xc-=2;
			}
			x[xc]++;
			xc+=2;
		}
		xc++;
		x[xc]++;
		xc++;
		while(x[xc] != 0)
		{
			xc++;
			x[xc]+=5;
			while(x[xc] != 0)
			{
				xc++;
				x[xc]+=2;
				xc++;
				x[xc]+=2;
				xc-=2;
				x[xc]--;
			}
			xc--;
			x[xc]--;
		}
		xc+=5;
		while(x[xc] != 0)
		{
			xc-=3;
			x[xc]++;
			xc++;
			x[xc]++;
			xc++;
			x[xc]++;
			xc++;
			x[xc]--;
		}
		xc--;
		x[xc]--;
		while(x[xc] != 0)
		{
			xc++;
			x[xc]++;
			xc--;
			x[xc]--;
		}
		x[xc]++;
		xc-=2;
		while(x[xc] != 0)
		{
			xc--;
			while(x[xc] != 0)
			{
				xc--;
				x[xc]++;
				xc++;
				x[xc]--;
			}
			xc-=2;
			while(x[xc] != 0)
			{
				xc+=2;
				x[xc]++;
				xc-=2;
				x[xc]--;
			}
			xc++;
			while(x[xc] != 0)
			{
				xc--;
				x[xc]++;
				xc++;
				x[xc]--;
			}
			xc+=2;
			x[xc]--;
		}
		xc-=3;
		while(x[xc] != 0)
		{
			x[xc]--;
		}
		xc+=2;
		while(x[xc] != 0)
		{
			xc--;
			x[xc]++;
			xc++;
			xc+=2;
			x[xc]--;
			xc-=2;
			x[xc]--;
		}
		x[xc]+=6;
		while(x[xc] != 0)
		{
			xc++;
			x[xc]+=8;
			xc--;
			x[xc]--;
		}
		xc+=2;
		while(x[xc] != 0)
		{
			xc--;
			x[xc]++;
			xc++;
			x[xc]-=2;
		}
		xc--;
		putchar(x[xc]);
		while(x[xc] != 0)
		{
			x[xc]--;
		}
		xc+=2;
		while(x[xc] != 0)
		{
			xc+=3;
		}
		xc+=2;
	}
	printf("\n");
}
