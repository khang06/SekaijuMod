/******************************************************************************/
/*                                                                            */
/*  MIDIClock���C�u�����u�͂��߂ɂ��ǂ݂��������v          (C)2002-2014 ����  */
/*                                                                            */
/******************************************************************************/

�@���̂��т́AMIDIClock���C�u�������_�E�����[�h���Ă��������A���͂��󂯎��ɂ�
���Ă��������A���ɂ��肪�Ƃ��������܂��B

�@MIDIClock���C�u�����́A�t���[�ŃI�[�v���\�[�X�́AMIDI�����v���p���C�u�����ł��B
���̃��C�u�����́AMIDI�N���b�N�̃X�^�[�g�E�X�g�b�v�A�e���|�̎擾�E�ݒ�A�o�߃e�B
�b�N���̎擾�E�ݒ�A�o�ߎ����̎擾�E�ݒ�ȂǁAMIDI�̃^�C�~���O����舵����ŕK�v
�s���ȋ@�\��񋟂��܂��B

�y�����z

1.���ʂȃ^�C���x�[�X�ƃe���|�ɑΉ��B
�@�^�C���x�[�X��TPQN�x�[�X(����\=1�`32767)��SMPTE24�x�[�X(24�t���[��/�b)�ESMPTE25
�x�[�X(25�t���[��/�b)�ESMPTE29�x�[�X(29.97�t���[��/�b)�ESMPTE30�x�[�X(30�t���[��
/�b)�ɑΉ����ATPQN�x�[�X�̏ꍇ�̃e���|��1�`60000000[�ʕb/4������]�ɑΉ��B�����
�̐ݒ�l�̓��A���^�C���Ɏ擾�E�ύX�ł��܂��B�����͌o�߃e�B�b�N�l�ƌo�߃~���b�l��
���������A���^�C���Ɏ擾�E�ݒ�ł��܂��B

2.�����x�ȃe�B�b�N�W�F�l���[�^�𓋍�
�@1�~���b�P�ʂ̃}���`���f�B�A�^�C�}�[�̗̍p�A�e�B�b�N�v�Z�덷���Z�A���S���Y����
�̗p�ɂ��A�������x���������Ă��܂��B

3.�X���[�u���[�h�ɂ��Ή��B
�@�������g�Ŏ�����i�s������̂ł͂Ȃ��AMIDI�^�C�~���O�N���b�N��SMPTE/MTC��F��
�����邱�Ƃɂ���Ď�����i�s������X���[�u���[�h�����ځB�O���@����}�X�^�[�N���b
�N�Ƃ���@�\���������A�v���P�[�V����������ł��܂��B

�y�Y�t�t�@�C���z

MIDIClock
��readme.txt              �͂��߂ɂ��ǂ݂�������(���̃t�@�C��)
��license.txt             ���C�Z���X(�����E�p��)
��MIDIClock.c             C�\�[�X�t�@�C��
��MIDIClock.h             C/C++�p�w�b�_�[�t�@�C��
��MIDIClock.def           C/C++�p���W���[����`�t�@�C��(dll�����Ƃ��ɕK�v)
��MIDIClock.mak           C/C++�p���C�N�t�@�C��
��MIDIClock.sln           Visual C++ 2008 Service Pack 1�p�\�����[�V�����t�@�C��
��MIDIClock.vcproj        Visual C++ 2008 Service Pack 1�p�v���W�F�N�g�t�@�C��
��MIDIClock.bas           Visual Basic 4.0/5.0/6.0�p�C���|�[�g���W���[��
��Debug
����MIDIClockd.lib        �I�u�W�F�N�g���C�u�������W���[��(�f�o�b�O�p)
����MIDIClockd.dll        �_�C�i�~�b�N�����N���C�u����(�f�o�b�O�p)
��Release
����MIDIClock.lib         �I�u�W�F�N�g���C�u�������W���[��(�����[�X�p)
����MIDIClock.dll         �_�C�i�~�b�N�����N���C�u����(�����[�X�p)
��docs
�@��MIDIClock.html        �����K�C�h�u�b�N
�@��MIDIClock_master.gif  �����K�C�h�u�b�N�Ŏg���Ă���}
�@��MIDIClock_slave.gif   �����K�C�h�u�b�N�Ŏg���Ă���}
�@��MIDIClock_o01.gif     �����K�C�h�u�b�N�Ŏg���Ă���}
�@��MIDIClock_o02.gif     �����K�C�h�u�b�N�Ŏg���Ă���}
�@��MIDIClock_o02f.gif    �����K�C�h�u�b�N�Ŏg���Ă���}
�@��MIDIClock_o04.gif     �����K�C�h�u�b�N�Ŏg���Ă���}
�@��MIDIClock_o04f.gif    �����K�C�h�u�b�N�Ŏg���Ă���}
�@��MIDIClock_o08.gif     �����K�C�h�u�b�N�Ŏg���Ă���}
�@��MIDIClock_o08d.gif    �����K�C�h�u�b�N�Ŏg���Ă���}
�@��MIDIClock_o16.gif     �����K�C�h�u�b�N�Ŏg���Ă���}
�@��MIDIClock_o16f.gif    �����K�C�h�u�b�N�Ŏg���Ă���}
�@��MIDIClock_o32.gif     �����K�C�h�u�b�N�Ŏg���Ă���}
�@��MIDIDR02.gif          �����K�C�h�u�b�N�Ŏg���Ă���}


�y�g�p���@�z

�E���ׂẴt�@�C�����𓀂��Ă��������B
�EMIDIClock.h��VisualC++���C���X�g�[�������t�H���_����include�t�H���_���ɃR�s�[���Ă��������B
�EMIDIClock.lib��VisualC++���C���X�g�[�������t�H���_����lib�t�H���_���ɃR�s�[���Ă��������B
�EMIDIClock.dll�t�@�C����c:\windows\System32\��(32bitOS�̏ꍇ)�A
�@c:\windows\SysWOW64\��(64bitOS�̏ꍇ)�ɃR�s�[���Ă��������B
�EMIDIClock���C�u�������g�p����\�[�X�ł́A#include "MIDIClock.h"���s���Ă��������B
�EMIDIClock���C�u�������g�p����v���W�F�N�g�ł́AVisualC++�����g�p�̏ꍇ�A
�E�u�r���h(B)�v-�u(�v���W�F�N�g��)�̃v���p�e�B�v����u(�v���W�F�N�g��)�̃v���p�e�B�y�[�W�v�Ƃ���
�@�_�C�A���O��\�������A���̒��́u�\���ƃv���p�e�B�v�́u�����J�v�́u���́v�̒��ɁA
�@�u�ǉ��̈ˑ��t�@�C���v�Ƃ������ڂ�����̂ŁA������MIDIClock.lib��ǉ����Ă��������B
�E�A�v���P�[�V������z�z����ۂɂ́AMIDIClock.dll��exe�t�@�C���Ɠ����t�H���_�ɓY�t���Ă��������B
�E�ڂ����́A���[�Ղ�MIDI�Ղ낶�����Ƃ̎�������FAQ�̃y�[�W���Q�Ƃ��Ă��������B

�y���C�Z���X�z

�@���̃��C�u�����́AGNU �򓙈�ʌ��O���p�����_��(LGPL)�Ɋ�Â��Ĕz�z����܂��B
�E���Ȃ��͂��̃��C�u�������ALGPL�Ɋ�Â��A�����E�]�ځE�z�z���邱�Ƃ��ł��܂��B
�E���Ȃ��͂��̃��C�u���������ς��邱�Ƃ��ł��A�����LGPL�Ɋ�Â��z�z���邱�Ƃ��ł��܂��B
�E���Ȃ��͂��̃��C�u������DLL�𗘗p����(�_�C�i�~�b�N�����N�Ɍ���)�A���Ȃ��Ǝ���
�@���C�Z���X�̃A�v���P�[�V�����𐻍�E�z�z���邱�Ƃ��ł��܂��B
�E������̏ꍇ����҂ɋ��𓾂�K�v�͂���܂���B
�E���̃��C�u�����͑S���̖��ۏ؂ł��B
�@���̃��C�u�������g�p�������ʐ��������Q�ɂ��܂��č�҂͈�ؐӔC�𕉂��܂���B
�@���炩���߂��������������B
���ڂ����́ALGPL�̓��{���(http://www.opensource.gr.jp/lesser/lgpl.ja.html)�����Q�Ƃ��������B

�y�A����z

�E���[���A�h���X(��) ee65051@yahoo.co.jp
�E�v���W�F�N�g�z�[���y�[�W http://openmidiproject.sourceforge.jp/index.html


